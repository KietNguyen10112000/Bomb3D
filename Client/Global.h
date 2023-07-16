#pragma once

#include "Network/TCPConnector.h"

#include "Synch/Package.h"
#include "Synch/Action.h"

#include "GameSetting.h"
#include "GameMap.h"
#include "Core/Random/ProcRandom.h"

#include "TeamInfo.h"

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

	size_t activeTeamCount = 2;
	size_t activePlayerCount = 0;
	size_t userId = 0;

	Scene2D* activeScene;
	GameSetting setting;
	GameMap gameMap;
	Camera2D* cam;
	PlayerScript* players[GameConfig::MAX_PLAYERS] = {};

	ProcRandom procRandom = {};

	TeamInfo teamsInfo[2] = {};
	bool isGameOver = false;

	inline auto GetMyPlayer()
	{
		return players[userId];
	}

	inline auto GetMyTeamId()
	{
		return userId % 2;
	}

	inline auto& GetMyTeam()
	{
		return teamsInfo[GetMyTeamId()];
	}

	inline auto& GetOppositeTeam()
	{
		return teamsInfo[(GetMyTeamId() + 1) % 2];
	}

	inline void ExecuteAction(Action* action)
	{
		actionCount++;
		actionStream->Put<ActionID>(action->GetActionID());
		action->Serialize(*actionStream);
	}

	inline auto& Random()
	{
		return procRandom;
	}
};