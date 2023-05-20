## Prerequisite:

+ Install python 3+

+ Install CMake

+ Install Visual Studio C/C++ toolchain

+ Install SoftEngine
  
  + `git clone git@github.com:KietNguyen10112000/SoftEngine.git`
  
  + `cd SoftEngine`
  
  + `git checkout 2D_SFML`
  
  + `python build.py`
  
  + Set system environment variable: `SOFT_ENGINE_HOME=<path to git cloned above directory>`

## How to run the game:

+ Open `*.sln` file with Visual Studio then build `Server`, `Client`

+ Copy directory `Resources` to SoftEngine executable directory (`SOFT_ENGINE_HOME/build/bin/<Debug/Release>`)

+ Copy `_Server.bat`, `_Client.bat` to SoftEngine executable directory, run 1 instance `_Server.bat` + 2 instances `_Client.bat`

+ Or run direct `Server` and `Client` from Visual Studio project
