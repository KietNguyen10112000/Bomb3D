#pragma once

#include "Global.h"

#include <pybind11/embed.h>

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(path, m) {
    m.def("find", [](int x, int y, int layerId = 0) {
        Global::Get().gameMap.m_pathFinder.Setup(
            Global::Get().activeScene->GetIterationCount(),
            Global::Get().gameMap.m_movable,
            Global::Get().gameMap.m_width,
            Global::Get().gameMap.m_height,
            PathFinder::Cell(x, y)
        );

        /*auto passingHandle = mheap::New<Scene2D::TimeHandle>();
        auto handle = Global::Get().activeScene->SetInterval(0.32f, 
            [](const Handle<Scene2D::TimeHandle>& handle)
            {
                if (Global::Get().gameMap.m_pathFinder.Find(
                        Global::Get().activeScene->GetIterationCount(), 100))
                {
                    Global::Get().activeScene->ClearInterval(*handle.Get());
                }
            },
            passingHandle
        );*/

       
        std::cout << "Oke\n";
        auto handle = Global::Get().activeScene->SetInterval(0.32f, 
            []()
            {
                std::cout << "Hello\n";
            }
        );

    });
}