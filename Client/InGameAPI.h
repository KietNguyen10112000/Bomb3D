#pragma once

bool* GetPathDebugVar(size_t id);
void FindPath(size_t id, int x, int y);

class PlayerScript;
PlayerScript* GetMyPlayer();

struct InGameAPIPlayerData
{
	size_t coin = -1;
	float hp = -1;
	float speed = -1;
};
void SetPlayerData(PlayerScript* player, const InGameAPIPlayerData& data);