#pragma once

#include "Network/TCPConnector.h"

#include "Synch/Package.h"

using namespace soft;

class GameLoopHandler;

struct Global
{
	static Global* s_instance;

	inline static auto& Get()
	{
		return *s_instance;
	}

	float fixedDt = 0.016f;
	GameLoopHandler* gameLoop = nullptr;

	TCPConnector*		connector;
	PackageSender		sender;
	ByteStream			sendStream;
};