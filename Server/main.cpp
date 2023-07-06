#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "TaskSystem/TaskSystem.h"

#ifdef _DEBUG
//#define SERVER_REPL
#endif // _DEBUG

#include "Global.h"

#include "GameActions/GameActions.h"

#include "ServerConfig.h"

constexpr static size_t TIME_IDLE = 15; // ms

void InitConsole(Engine* engine)
{
#ifdef SERVER_REPL
	std::cout << "[Python]> ";
	Task task;
	task.Entry() = [](void* e) 
	{
		Engine* engine = (Engine*)e;

		std::string line;
		line.reserve(2048);
		while (true)
		{
			std::getline(std::cin, line);

			if (line == "quit")
			{
				engine->IsRunning() = false;
				break;
			}

			String str = line.c_str();
			Global::Get().replStr.enqueue(str);
		}
	};
	task.Params() = engine;

	TaskSystem::Submit(task, Task::CRITICAL);

#endif // SERVER_REPL
}

#ifdef SERVER_REPL
inline void EvalStr(const String& str)
{
	std::cout << str << "\n";
}

inline void ConsumeRepl()
{
	auto& queue = Global::Get().replStr;
	String str;
	bool eval = false;
	while (queue.size_approx() != 0)
	{
		if (!queue.try_dequeue(str))
		{
			return;
		}

		EvalStr(str);
		eval = true;
	}

	if (eval)
		std::cout << "[Python]> ";
}
#endif // SERVER_REPL


class ServerLoopHandler;

Global* Global::s_instance = nullptr;

inline bool IsFullRoom()
{
	return Global::Get().gameRoomCount.load(std::memory_order_relaxed) == Global::MAX_ROOMS;
}

inline void FindNextRoomIdx()
{
	for (size_t i = 0; i < Global::MAX_ROOMS; i++)
	{
		auto idx = (i + Global::Get().gameRoomIdx) % Global::MAX_ROOMS;

		if (Global::Get().gameRoom[idx].m_id == INVALID_ID)
		{
			Global::Get().gameRoomIdx = idx;
			return;
		}
	}
	Global::Get().gameRoomIdx = INVALID_ID;
}

inline auto& GetCurrentMatchingRoom()
{
#ifdef _DEBUG
	//FindNextRoomIdx();
	for (size_t i = 0; i < Global::MAX_ROOMS; i++)
	{
		auto idx = i % Global::MAX_ROOMS;

		if (Global::Get().gameRoom[idx].m_id == INVALID_ID)
		{
			Global::Get().gameRoomIdx = idx;
			break;
		}
	}
	return Global::Get().gameRoom[Global::Get().gameRoomIdx];
#else
	if (Global::Get().gameRoom[Global::Get().gameRoomIdx].m_id == INVALID_ID)
	{
		FindNextRoomIdx();
	}
	return Global::Get().gameRoom[Global::Get().gameRoomIdx];
#endif // _DEBUG
}

inline auto NextRoomIdx()
{
	Global::Get().gameRoomCount++;
	Global::Get().gameRoomIdx++;
	FindNextRoomIdx();
}

inline void AcceptClient(void*)
{
	if (!IsFullRoom())
	{
		auto& room = GetCurrentMatchingRoom();
		auto& conn = room.GetClientConn(room.m_clientsCount);
		auto ret = Global::Get().acceptor.Accept(conn);

		if (ret == 0)
		{
			//room.InitializeClient(room.m_clientsCount);
			conn.SetBlockingMode(false);
			std::cout << "Client connected\n";
			room.m_clientsCount++;

			if (room.m_clientsCount == ServerConfig::NUM_PLAYER_PER_ROOM)
			{
				room.m_abortCallback = [](void*)
				{
					Global::Get().gameRoomCount--;
				};
				room.StartUp(Global::Get().gameRoomIdx);
				room.m_id = Global::Get().gameRoomIdx;
				NextRoomIdx();
			}
		}
	}

	Global::Get().acceptClientLock.exchange(false, std::memory_order_release);
}

class ServerLoopHandler : public IterationHandler
{
	/*inline void UpdateAllRoom()
	{
		auto fixedDt = Global::Get().fixedDt;
		for (size_t i = 0; i < Global::MAX_ROOMS; i++)
		{
			auto& room = Global::Get().gameRoom[i];
			if (room.m_id == INVALID_ID)
			{
				continue;
			}

			room.FixedIteration(fixedDt);
		}
	}*/

	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
		//auto cur = Clock::ms::now();

#ifdef SERVER_REPL
		ConsumeRepl();
#endif // SERVER_REPL

		if (Global::Get().acceptClientLock.load(std::memory_order_relaxed) == false
			&& Global::Get().acceptClientLock.exchange(true, std::memory_order_acquire) == false)
		{
			Task task;
			task.Entry() = AcceptClient;
			TaskSystem::Submit(task, Task::CRITICAL);
		}

		struct UpdateTaskParams
		{
			float sumDt = 0;
			size_t startIdx = 0;
		};

		static UpdateTaskParams		updateParams		[Global::NUM_UPDATE_TASK];
		static Task					updateTasks			[Global::NUM_UPDATE_TASK];
		static size_t				globalIterationCount = 1;

		globalIterationCount++;
		if (globalIterationCount == 0)
		{
			globalIterationCount = 1;
		}

		for (size_t i = 0; i < Global::NUM_UPDATE_TASK; i++)
		{
			auto& params	= updateParams[i];
			auto& task		= updateTasks[i];

			params.startIdx = i * Global::MAX_ROOMS / Global::NUM_UPDATE_TASK;
			params.sumDt = sumDt;

			task.Entry() = [](void* p)
			{
				TASK_SYSTEM_UNPACK_PARAM_2(UpdateTaskParams, p, sumDt, startIdx);

				auto start = Clock::ms::now();

				((UpdateTaskParams*)p)->sumDt = GameRoom::UpdateAllRooms(globalIterationCount, 
					Global::Get().fixedDt, sumDt, startIdx, Global::Get().gameRoom, Global::MAX_ROOMS);

				auto dt = Clock::ms::now() - start;
				if (dt < TIME_IDLE)
				{
					Thread::Sleep(TIME_IDLE - dt);
				}
			};
			task.Params() = &params;
		}

		TaskSystem::SubmitAndWait(updateTasks, Global::NUM_UPDATE_TASK, Task::CRITICAL);
		//updateTasks[0].Entry()(&updateParams[0]);
		return updateParams->sumDt;
	}
};

void InitializeSendTasks(Engine* engine)
{
	Task tasks[Global::NUM_SEND_TASK];
	for (auto& task : tasks)
	{
		task.Entry() = [](void* p)
		{
			auto e = (Engine*)p;
			auto startIdx = Global::Get().sendLoopIdx.fetch_add(Global::MAX_ROOMS / Global::NUM_SEND_TASK);
			GameRoom::SendLoop<TIME_IDLE>(&e->IsRunning(), startIdx, Global::Get().gameRoom, Global::MAX_ROOMS);
		};
		task.Params() = engine;
	}
	TaskSystem::Submit(tasks, Global::NUM_SEND_TASK, Task::CRITICAL);
}

void Initialize(Engine* engine)
{
	GameActions::InitializeAllGameActions();

	Global::s_instance = new Global();
	Global::Get().serverLoop = new ServerLoopHandler();
	engine->SetIterationHandler(Global::Get().serverLoop);

	InitializeSendTasks(engine);

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = "0.0.0.0";
	desc.port = 9023;
	desc.useNonBlocking = true;
	Global::Get().acceptor.Initialize(desc, Global::MAX_ROOMS * GameRoom::MAX_CLIENTS);

	InitConsole(engine);
}

void Finalize(Engine* engine)
{
	delete Global::Get().serverLoop;
	delete Global::s_instance;
}