#pragma once

#include "Global.h"

#include <pybind11/embed.h>
#include <pybind11/functional.h>

namespace py = pybind11;

PYBIND11_EMBEDDED_MODULE(game, m) {
    class PathDebugAccessor
    {
    public:
        inline bool GetDebug() const
        {
            return Global::Get().setting.isDebugPathfinder;
        }

        inline bool SetDebug(bool debug) const
        {
            return (Global::Get().setting.isDebugPathfinder = debug);
        }

        inline int GetStep() const
        {
            return Global::Get().setting.pathFinderUpdateStep;
        }

        inline int SetStep(int debug) const
        {
            return (Global::Get().setting.pathFinderUpdateStep = debug);
        }

        inline void FindPath(int x, int y)
        {
            auto layer = Global::Get().gameMap.m_pathFinder.Find(
                Global::Get().activeScene->GetIterationCount(),
                PathFinder::Cell(x, y)
            );

            auto passingHandle = mheap::New<Scene2D::TimeHandle>();
            auto handle = Global::Get().activeScene->SetInterval(0.32f,
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
    };

    py::class_<PathDebugAccessor>(m, "PathDebugAccessor")
        .def(py::init<>())
        .def_property("debug", &PathDebugAccessor::GetDebug, &PathDebugAccessor::SetDebug)
        .def_property("step", &PathDebugAccessor::GetStep, &PathDebugAccessor::SetStep)
        .def("find",
            [](PathDebugAccessor& self, int x, int y)
            {
                self.FindPath(x, y);
            }
        );
}

PYBIND11_EMBEDDED_MODULE(engine, m) {
    class _Engine {};
    m.def("setTimeout", [](std::function<void()>& callback, int delay)
            {
                Global::Get().activeScene->SetTimeout(delay / 1000.0f,
                    [callback]()
                    {
                        callback();
                    }
                );
            }
        );
}


inline void BindPython()
{
    py::exec(R"#(
        import engine
        import game
        game.path = game.PathDebugAccessor()
    )#");
}