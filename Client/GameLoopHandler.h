#pragma once

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"
#include "Objects2D/Scene2D/Scene2D.h"

#include "Global.h"

#include "Synch/Action.h"

#include "GameActions/GameActions.h"
#include "GameActions/UserInputAction.h"
#include "GameActions/GameActionConfig.h"
#include "GameActions/MatchStartAction.h"

#include "GameInit.h"

using namespace soft;

class GameLoopHandler : public IterationHandler
{
	constexpr static size_t NUM_STREAM = 32;

	constexpr static size_t MAX_DELAY_ITERATION = 4;

	constexpr static size_t NUM_ITERATION_PER_SEND = GameActionConfig::ClientConfig::NUM_TICKS_PER_SYNCH;

	struct ActionSynch
	{
		Action* action = nullptr;
		size_t iteration;
	};

	// max allowed iteration, this field received from server
	size_t m_maxIteration = 0;

	size_t m_iterationCount = 0;

	size_t m_sendIteration = 0;

	
	ByteStream						m_sendStreams		[NUM_STREAM];
	Spinlock						m_sendStreamsLock	[NUM_STREAM];
	ConcurrentQueue<ByteStream*>	m_sendActions;
	PackageSender					m_sender;

	ByteStream*						m_curSendStream = &m_sendStreams[0];
	size_t							m_curSendStreamIdx = 0;

	/*ByteStream						m_recvStreams		[NUM_STREAM];
	Spinlock						m_recvStreamsLock	[NUM_STREAM];*/
	ConcurrentQueue<ActionSynch>	m_recvActions;
	PackageReceiver					m_receiver;

	ActionSynch						m_actionSynch;

	bool							m_isTransportRunning = true;
	bool							m_isReachedEndOfTransport = false;
	bool							m_isMatchedSuccess = false;

public:
	UserInput m_userInput[10];

	~GameLoopHandler()
	{
		m_isTransportRunning = false;

		while (m_isReachedEndOfTransport == false)
		{
			Thread::Sleep(1);
		}

		if (m_actionSynch.action)
		{
			ActionCreator::Delete(m_actionSynch.action);
		}
		while (true)
		{
			if (!m_recvActions.try_dequeue(m_actionSynch))
			{
				break;
			}

			ActionCreator::Delete(m_actionSynch.action);
		}
	}

private:
	inline void FixedIteration(float dt, Scene2D* scene)
	{
		Global::Get().actionCount = 0;
		Global::Get().actionStream = m_curSendStream;

		m_iterationCount++;
		m_curSendStream->Put<size_t>(m_iterationCount);
		auto pActionCountIdx = m_curSendStream->Put<uint32_t>(0);

		ComsumeAction(scene);
		scene->LockDt(dt);

		Global::Get().gameMap.m_pathFinder.Update(
			Global::Get().activeScene->GetIterationCount(), 
			Global::Get().setting.pathFinderUpdateStep
		);

		scene->PrevIteration();
		scene->Iteration();

		m_curSendStream->Set(pActionCountIdx, Global::Get().actionCount);

		//std::cout << "Record " << Global::Get().actionCount << " actions\n";

		//SendUserAction();
	}

	inline void ComsumeAction(Scene2D* scene)
	{
		if (m_actionSynch.action)
		{
			assert(m_iterationCount <= m_actionSynch.iteration);

			if (m_actionSynch.iteration == m_iterationCount)
			{
				m_actionSynch.action->Activate(scene);
				ActionCreator::Delete(m_actionSynch.action);
				m_actionSynch.action = nullptr;
			}
			else
			{
				return;
			}
		}

		while (true)
		{
			if (!m_recvActions.try_dequeue(m_actionSynch))
			{
				break;
			}

			assert(m_iterationCount <= m_actionSynch.iteration);

			if (m_actionSynch.iteration == m_iterationCount)
			{
				m_actionSynch.action->Activate(scene);
				ActionCreator::Delete(m_actionSynch.action);
				m_actionSynch.action = nullptr;
			}
			else
			{
				break;
			}
		}
	}

	inline void SendUserAction()
	{
		m_sendIteration++;
		if (m_sendIteration % NUM_ITERATION_PER_SEND == 0)
		{
			m_sendActions.enqueue(m_curSendStream);
			m_curSendStreamIdx = (m_curSendStreamIdx + 1) % NUM_STREAM;
			m_curSendStream = &m_sendStreams[m_curSendStreamIdx];
			m_sendStreamsLock[m_curSendStreamIdx].lock_no_check_own_thread();
		}
	}

public:
	ByteStream* m_curSendingStream = nullptr;
	inline void TransportLoop()
	{
		auto& conn = *Global::Get().connector;

		while (m_isTransportRunning)
		{
			bool allowSleep = true;
			auto start = Clock::ms::now();
			//m_maxIteration++;
			while (true)
			{
				if (m_curSendingStream == nullptr && !m_sendActions.try_dequeue(m_curSendingStream))
				{
					break;
				}

				//std::cout << "Send pkg: " << userStream->GetPayloadSize() << " bytes\n";
				auto sendRet = m_sender.TrySend(*m_curSendingStream, conn);
				//auto sendRet = m_sender.TrySendTest(*m_curSendingStream, conn);

				if (sendRet == PackageSender::ERCODE::SUCCEEDED)
				{
					auto lockedStreamIdx = m_curSendingStream - m_sendStreams;
					m_sendStreams[lockedStreamIdx].Clean();
					m_sendStreamsLock[lockedStreamIdx].unlock_no_check_own_thread();
					m_curSendingStream = nullptr;
				}
				else if (sendRet == PackageSender::ERCODE::CONNECTION_BUSY)
				{
					allowSleep = false;
					break;
				}
				else if (sendRet == PackageSender::ERCODE::CONNECTION_ERROR)
				{
					std::cout << "Connection error. Aborted.\n";
					Throw();
				}
			}

			while (true)
			{
				auto recvRet = m_receiver.TryRecv(conn);
				if (recvRet == PackageReceiver::ERCODE::SUCCEEDED)
				{
					goto RecvSucceeded;
				}
				else if (recvRet == PackageReceiver::ERCODE::CONNECTION_BUSY)
				{
					allowSleep = false;
					break;
				}
				else if (recvRet == PackageReceiver::ERCODE::CONNECTION_ERROR)
				{
					//std::cout << "Connection error. Aborted.\n";
					//Throw();
					break;
				}

			RecvSucceeded:
				auto serverStream = m_receiver.GetStream();

				while (!serverStream.IsEmpty())
				{
					auto iteration = serverStream.Get<size_t>();
					auto numAction = serverStream.Get<uint32_t>();
					//std::cout << "Recv iteration " << iteration << "\n";
					for (size_t i = 0; i < numAction; i++)
					{
						ActionID aId = serverStream.Get<ActionID>();
						auto action = ActionCreator::New(aId);
						action->Deserialize(serverStream);
						m_recvActions.enqueue({ action, iteration });
					}

					assert(m_maxIteration < iteration);
					m_maxIteration = iteration;
				}
			}

			auto dt = Clock::ms::now() - start;
			if (allowSleep && dt < 15)
			{
				Thread::Sleep(15 - dt);
			}
		}

	Return:
		m_isReachedEndOfTransport = true;
	}

	inline void WaitForMatchStart(Engine* engine)
	{
		std::cout << "Wait for match start...\n";
		while (true)
		{
			if (!m_recvActions.try_dequeue(m_actionSynch))
			{
				Thread::Sleep(16);
				continue;
			}

			assert(m_actionSynch.action->GetActionID() == GameActions::ACTION_ID::MATCH_START);

			engine->AddListener(ENGINE_EVENT::SCENE_ON_SETUP,
				[](int argc, void** argv, void* a)
				{
					MatchStartAction* matchStart = (MatchStartAction*)a;
					AddStaticObjects((Scene2D*)argv[1], 
						matchStart->m_map, matchStart->m_width, matchStart->m_height,
						matchStart->m_blockCellValues, matchStart->m_numBlockCell);

					Global::Get().gameMap.Initialize(matchStart);

					Global::Get().activeScene = (Scene2D*)argv[1];
				},
				m_actionSynch.action
			);

			engine->AddListener(ENGINE_EVENT::SCENE_ON_START,
				[](int argc, void** argv, void* a)
				{
					auto scene = (Scene2D*)argv[1];
					MatchStartAction* matchStart = (MatchStartAction*)a;

					String ownIP = Global::Get().connector->GetAddressString();
					std::cout << "Player address: " << ownIP << "\n";

					auto& clients = matchStart->m_clientInfo;
					auto clientCount = matchStart->m_numClient;
					for (size_t i = 0; i < clientCount; i++)
					{
						auto& client = clients[i];
						if (i == matchStart->m_userID)
						{
							std::cout << "Player id: " << client.id << "\n";
							Global::Get().userId = client.id;
						}
					}

					for (size_t i = 0; i < clientCount; i++)
					{
						auto& client = clients[i];
						AddPlayer(scene, client.id, client.pos, matchStart->m_width, matchStart->m_height);
					}
					
					AddMapRenderer(scene, matchStart->m_map, matchStart->m_width, matchStart->m_height);

					AddUINode(scene);

					// response for server know we're ready for game
					auto& sender = Global::Get().gameLoop->m_sender;
					auto& stream = Global::Get().gameLoop->m_sendStreams[0];
					stream.Clean();

					// iteration count
					stream.Put<size_t>(0);
					// num actions
					stream.Put<uint32_t>(1);
					// action
					stream.Put<ActionID>(matchStart->GetActionID());
					matchStart->Clear();
					matchStart->SetUserId(Global::Get().userId);
					matchStart->Serialize(stream);

					sender.SendSynch(stream, *Global::Get().connector);
					stream.Clean();

					ActionCreator::Delete(matchStart);
				},
				m_actionSynch.action
			);
			
			m_actionSynch.action = nullptr;
			break;
		}
	}

	inline void StartUp(Engine* engine)
	{
		for (auto& stream : m_sendStreams)
		{
			stream.Initialize(64 * KB);
		}

		m_receiver.Initialize();

		Task task;
		task.Entry() = [](void* p)
		{
			Global::Get().gameLoop->TransportLoop();
		};
		TaskSystem::Submit(task, Task::CRITICAL);

		m_sendStreamsLock[m_curSendStreamIdx].lock_no_check_own_thread();

		WaitForMatchStart(engine);
		m_maxIteration = 0;
		m_iterationCount = 0;
	}

	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
		auto fixedDt = Global::Get().fixedDt;

		auto iter = m_iterationCount;

		assert(m_maxIteration >= m_iterationCount);

		auto maxIteration = m_maxIteration;

		if (maxIteration != m_iterationCount)
		{
			if (maxIteration - m_iterationCount > MAX_DELAY_ITERATION)
			{
				auto loopCount = maxIteration - m_iterationCount - MAX_DELAY_ITERATION;
				for (size_t i = 0; i < loopCount; i++)
				{
					FixedIteration(fixedDt, scene);
				}
			}
		}

		while (sumDt > fixedDt)
		{
			if (maxIteration != m_iterationCount)
			{
				FixedIteration(fixedDt, scene);
			}
			sumDt -= fixedDt;
		}

		if (iter != m_iterationCount)
		{
			SendUserAction();
		}

		scene->PostIteration();

		return sumDt;
	}

public:
	inline void OnRecvMatchStart(MatchStartAction& match)
	{
		m_isMatchedSuccess = true;
	}

};