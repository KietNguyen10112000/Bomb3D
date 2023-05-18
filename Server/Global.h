#pragma once

#include "GameRoom.h"

#ifdef SERVER_REPL
#include "Core/Structures/Raw/ConcurrentQueue.h"
#include "Core/Structures/String.h"
#endif // SERVER_REPL

using namespace soft;

class ServerLoopHandler;

struct Global
{
	static Global* s_instance;

	inline static auto& Get()
	{
		return *s_instance;
	}

	constexpr static size_t MAX_ROOMS = 128;

	float					fixedDt = 0.016f;
	ServerLoopHandler*		serverLoop = nullptr;

	GameRoom				gameRoom[MAX_ROOMS] = {};
	size_t					gameRoomCount = 0;

	TCPAcceptor				acceptor;

#ifdef SERVER_REPL
	ConcurrentQueue<String>	replStr = ConcurrentQueue<String>(128);
#endif // SERVER_REPL

};