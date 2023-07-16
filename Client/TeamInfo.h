#pragma once

struct TeamInfo
{
	size_t exp = 0;
	size_t aliveCount = 0;

	bool isVictory = false;
	bool hasVictoryTower = false;

	float remainVictoryTime = 0.0f;
};