#pragma once

#include "Engine/Engine.h"
#include "Objects2D/Scene2D/Scene2D.h"

#include "Global.h"

#include "Synch/Action.h"

#include "GameActions/UserInputAction.h"

using namespace soft;

class GameLoopHandler : public IterationHandler
{
	constexpr static size_t NUM_STREAM = 32;

	constexpr static size_t MAX_DELAY_ITERATION = 32;

	constexpr static size_t NUM_ITERATION_PER_SEND = 4;

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
	inline void TransportLoop()
	{
		auto& conn = *Global::Get().connector;
		ByteStream* userStream;

		while (m_isTransportRunning)
		{
			auto start = Clock::ms::now();
			//m_maxIteration++;
			while (true)
			{
				if (!m_sendActions.try_dequeue(userStream))
				{
					break;
				}

				//std::cout << "Send pkg: " << userStream->GetPayloadSize() << " bytes\n";
				m_sender.SendSynch(*userStream, conn);
				auto lockedStreamIdx = userStream - m_sendStreams;
				m_sendStreams[lockedStreamIdx].Clean();
				m_sendStreamsLock[lockedStreamIdx].unlock_no_check_own_thread();
				
				/*while (true)
				{
					Thread::Sleep(10000);
				}*/
			}

			while (true)
			{
				auto recvRet = m_receiver.RecvSynch(conn);

				if (recvRet == PackageReceiver::ERCODE::PACKAGE_EMPTY)
				{
					break;
				}
				else if (recvRet == PackageReceiver::ERCODE::CONNECTION_ERROR)
				{
					goto Return;
				}

				auto serverStream = m_receiver.GetStream();

				while (!serverStream.IsEmpty())
				{
					auto iteration = serverStream.Get<size_t>();
					auto numAction = serverStream.Get<uint32_t>();
					//std::cout << "Recv " << numAction << "\n";
					for (size_t i = 0; i < numAction; i++)
					{
						ActionID aId = serverStream.Get<ActionID>();
						auto action = ActionCreator::New(aId);
						action->Deserialize(serverStream);
						m_recvActions.enqueue({ action, iteration });
					}
					m_maxIteration = iteration;
				}
			}

			auto dt = Clock::ms::now() - start;
			if (dt < 15)
			{
				Thread::Sleep(15 - dt);
			}
		}

	Return:
		m_isReachedEndOfTransport = true;
	}

	inline void StartUp()
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
	}

	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
		auto fixedDt = Global::Get().fixedDt;

		auto iter = m_iterationCount;

		assert(m_maxIteration >= m_iterationCount);

		if (m_maxIteration != m_iterationCount)
		{
			if (m_maxIteration - m_iterationCount > MAX_DELAY_ITERATION)
			{
				auto loopCount = m_maxIteration - m_iterationCount - MAX_DELAY_ITERATION;
				for (size_t i = 0; i < loopCount; i++)
				{
					FixedIteration(fixedDt, scene);
				}
			}
		}

		while (sumDt > fixedDt)
		{
			if (m_maxIteration != m_iterationCount)
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
};