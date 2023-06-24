#include "InGameAPI.h"

#include "Global.h"

#include "Objects2D/Scene2D/Scene2D.h"

using namespace soft;

bool* GetPathDebugVar(size_t id)
{
    return &Global::Get().setting.isDebugPathfinder;
}

void FindPath(size_t id, int x, int y)
{
    auto layer = Global::Get().gameMap.m_pathFinder.Find(
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