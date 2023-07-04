#pragma once

#include "Network/TCPConnector.h"

#include "Synch/Package.h"
#include "Synch/Action.h"

#include "GameSetting.h"
#include "GameMap.h"

using namespace soft;

class GameLoopHandler;
class PlayerScript;

namespace soft 
{
	class Camera2D;
}

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

	Scene2D* activeScene;
	GameSetting setting;
	GameMap gameMap;
	Camera2D* cam;
	PlayerScript* players[10] = {};

	inline auto GetMyPlayer()
	{
		return players[userId];
	}

	inline auto GetMyTeamId()
	{
		return userId % 2;
	}

	inline void ExecuteAction(Action* action)
	{
		actionCount++;
		actionStream->Put<ActionID>(action->GetActionID());
		action->Serialize(*actionStream);
	}
};