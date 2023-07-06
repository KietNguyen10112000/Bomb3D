#ifdef CONSOLE

#include "BindPython.h"

#include <pybind11/embed.h>
#include <pybind11/functional.h>

namespace py = pybind11;

#include "InGameAPI.h"

PYBIND11_EMBEDDED_MODULE(game, m) {
    class PathDebugAccessor
    {
    public:
        inline bool GetDebug() const
        {
            return *GetPathDebugVar(0);
        }

        inline bool SetDebug(bool debug) const
        {
            return (*GetPathDebugVar(0) = debug);
        }

        inline void FindPath(int x, int y)
        {
            ::FindPath(0, x, y);
        }
    };

    py::class_<PathDebugAccessor>(m, "PathDebugAccessor")
        .def(py::init<>())
        .def_property("debug", &PathDebugAccessor::GetDebug, &PathDebugAccessor::SetDebug)
        .def("find",
            [](PathDebugAccessor& self, int x, int y)
            {
                self.FindPath(x, y);
            }
    );
}

//PYBIND11_EMBEDDED_MODULE(engine, m) {
//    class _Engine {};
//
//    py::class_<Scene2D::TimeHandle>(m, "TimeHandle")
//        .def(py::init<ID, ID>())
//        .def_readwrite("id", &Scene2D::TimeHandle::id)
//        .def_readwrite("uid", &Scene2D::TimeHandle::uid);
//
//    m.def("setTimeout", [](std::function<void()>& callback, int delay)
//        {
//            Global::Get().activeScene->SetTimeout(delay / 1000.0f,
//            [callback]()
//                {
//                    callback();
//                }
//    );
//        }
//    );
//
//    m.def("setInterval", [](std::function<void()>& callback, int delay)
//        {
//            auto id = Global::Get().activeScene->SetInterval(delay / 1000.0f,
//            [callback]()
//                {
//                    callback();
//                }
//    );
//
//    return id;
//        }
//    );
//
//    m.def("clearInterval", [](Scene2D::TimeHandle& handle)
//        {
//            Global::Get().activeScene->ClearInterval(handle);
//        }
//    );
//}


void BindPython()
{
    py::exec(R"#(
        #import engine
        import game
        game.path = game.PathDebugAccessor()
    )#");
}


#endif // CONSOLE