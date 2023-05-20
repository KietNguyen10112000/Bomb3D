#include "GameRoom.h"

#include "Global.h"

GameRoom* GameRoom::GetRoom(size_t id)
{
    return &Global::Get().gameRoom[id];
}
