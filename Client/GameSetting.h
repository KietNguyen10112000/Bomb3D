#pragma once

struct GameSetting
{
	enum class PlayerControlMode
	{
		NONE,
		DEBUG
	};

	PlayerControlMode		playerControlMode				= PlayerControlMode::NONE;
	bool					isOnConsole						= false;
	bool					isDebugPathfinder				= false;
	bool					isOnRClickToSpawnMonster		= true;
	bool					isCaptureSTDCout				= false;
	size_t					debugPathfinderLayer			= 0;
	size_t					pathFinderUpdateStep			= 500;

	bool					isShowChooseBuildingUI			= false;
	bool					isStopPlayerLeftMouse			= false;
};