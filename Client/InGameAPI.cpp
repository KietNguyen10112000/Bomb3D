#include "InGameAPI.h"

#include "Global.h"

#include "Objects2D/Scene2D/Scene2D.h"

#include "PlayerScript.h"

using namespace soft;

bool* GetPathDebugVar(size_t id)
{
    return &Global::Get().setting.isDebugPathfinder;
}

void FindPath(size_t id, int x, int y)
{
    auto idx = id % GameConfig::MAX_PLAYERS;

    auto** list = Global::Get().gameMap.m_playerPathFinders;
    if (id > GameConfig::MAX_PLAYERS)
    {
        list = Global::Get().gameMap.m_victoryTowersPathFinder;
    }

    auto pathFinder = list[idx];
    if (!pathFinder)
    {
        return;
    }

    auto layer = pathFinder->Find(
        Global::Get().activeScene->GetIterationCount(),
        PathFinder::Cell(x, y)
    );

    if (!layer)
    {
        return;
    }

    auto passingHandle = mheap::New<Scene2D::TimeHandle>();
    auto handle = Global::Get().activeScene->SetInterval(1.0f,
        [layer](const Handle<Scene2D::TimeHandle>& handle)
        {
            if (layer->IsFinished())
            {
                Global::Get().activeScene->ClearInterval(*handle);
                std::cout << "Done path finding\n";
            }
        },
        passingHandle
            );

    *passingHandle.Get() = handle;
}

PlayerScript* GetMyPlayer()
{
    return Global::Get().GetMyPlayer();
}

void SetPlayerData(PlayerScript* player, const InGameAPIPlayerData& data)
{
    if (data.hp != - 1)
        player->DynamicObjectProperties().hp = data.hp;

    if (data.speed != - 1)
        player->m_speed = data.speed;

    if (data.coin != -1)
        player->PlayerData().coin = data.coin;
}
