#include "Item.h"

#include "PlayerScript.h"

bool Coin::Use(PlayerScript* player)
{
	player->Data().coin += 10;
	return true;
}
