#pragma once

#include "Network/TCPConnector.h"

#include "Synch/Package.h"
#include "Synch/Action.h"

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
	
	uint32_t actionCount = 0;
	ByteStream* actionStream;

	size_t userId = 0;

	inline void ExecuteAction(Action* action)
	{
		actionCount++;
		actionStream->Put<ActionID>(action->GetActionID());
		action->Serialize(*actionStream);
	}
};