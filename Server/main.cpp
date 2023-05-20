#include "Plugins/Bridge/ScriptBridge.h"

#include "Engine/Engine.h"
#include "Engine/ENGINE_EVENT.h"

#include "TaskSystem/TaskSystem.h"

#define SERVER_REPL

#include "Global.h"

#include "GameActions/GameActions.h"

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

inline auto& GetCurrentMatchingRoom()
{
	return Global::Get().gameRoom[Global::Get().gameRoomIdx];
}

inline auto NextRoomIdx()
{
	Global::Get().gameRoomIdx++;
	for (size_t i = 0; i < Global::MAX_ROOMS; i++)
	{
		auto idx = (i + Global::Get().gameRoomIdx) % Global::MAX_ROOMS;

		if (Global::Get().gameRoom[idx].m_id == INVALID_ID)
		{
			Global::Get().gameRoomIdx = idx;
			break;
		}
	}
}

class ServerLoopHandler : public IterationHandler
{
	inline void UpdateAllRoom()
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
	}

	// Inherited via IterationHandler
	virtual float DoIteration(float sumDt, Scene2D* scene) override
	{
#ifdef SERVER_REPL
		ConsumeRepl();
#endif // SERVER_REPL

		auto& room = GetCurrentMatchingRoom();
		auto& conn = room.GetClientConn(room.m_clientsCount);
		auto ret = Global::Get().acceptor.Accept(conn);

		if (ret == 0)
		{
			room.InitializeClient(room.m_clientsCount);
			conn.SetBlockingMode(false);
			std::cout << "Client connected\n";
			room.m_clientsCount++;

			if (room.m_clientsCount == 2)
			{
				room.m_id = Global::Get().gameRoomIdx;
				room.StartUp();
				NextRoomIdx();
			}
		}

		auto fixedDt = Global::Get().fixedDt;
		while (sumDt > fixedDt)
		{
			UpdateAllRoom();
			sumDt -= fixedDt;
		}

		return sumDt;
	}
};


void Initialize(Engine* engine)
{
	GameActions::InitializeAllGameActions();

	Global::s_instance = new Global();
	Global::Get().serverLoop = new ServerLoopHandler();
	engine->SetIterationHandler(Global::Get().serverLoop);

	TCP_SOCKET_DESCRIPTION desc;
	desc.host = "0.0.0.0";
	desc.port = 9023;
	desc.useNonBlocking = true;
	Global::Get().acceptor.Initialize(desc, Global::MAX_ROOMS * GameRoom::MAX_CLIENTS);

	InitConsole(engine);
}

void Finalize(Engine* engine)
{
	std::cout << "Bye!\n";

	delete Global::Get().serverLoop;
	delete Global::s_instance;
}