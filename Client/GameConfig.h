#pragma once

#include "GameActions/GameActionConfig.h"

struct GameConfig
{
	constexpr static size_t CELL_SIZE = 60;
	constexpr static size_t MAX_PLAYERS = GameActionConfig::MAX_CLIENTS;
};