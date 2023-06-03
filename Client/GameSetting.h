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

};