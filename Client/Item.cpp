#include "Item.h"

#include "PlayerScript.h"

bool Coin::Use(PlayerScript* player)
{
	player->PlayerData().coin += 10;
	return true;
}
