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

	constexpr static size_t MAX_ROOMS				= 128;
	constexpr static size_t NUM_SEND_TASK			= 2;
	constexpr static size_t NUM_UPDATE_TASK			= 2;

	float					fixedDt = 0.016f;
	ServerLoopHandler*		serverLoop = nullptr;

	GameRoom				gameRoom[MAX_ROOMS] = {};
	size_t					gameRoomIdx = 0;
	std::atomic<size_t>		gameRoomCount = 0;

	TCPAcceptor				acceptor;

	std::atomic<size_t>		sendLoopIdx = 0;
	std::atomic<size_t>		recvLoopIdx = 0;

	std::atomic<bool>		acceptClientLock;

#ifdef SERVER_REPL
	ConcurrentQueue<String>	replStr = ConcurrentQueue<String>(128);
#endif // SERVER_REPL

};