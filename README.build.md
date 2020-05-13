# FLINT.example
This project is an example of how to build and run libraries using the FLINT framework and run these in a development environment.

## Environment: Visual Studio 16 2019 Win64

### Building the project

Assuming you have followed the moja flint documentation to build using the vcpkg method, the commands below should build your example project solution.

**NOTE**: Paths used in commands may be different on your system.

```powershell
#PowerShell

# Create a build folder under the Source folder
cd Source
mkdir build
cd build

cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=C:\Development\Software\moja -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_TESTS=OFF -DCMAKE_TOOLCHAIN_FILE=c:\Development\moja-global\vcpkg\scripts\buildsystems\vcpkg.cmake ..
```

### Running the project

Running in the IDE and debugging is a little tricky. This could more than likely be resolved with better cmake  setups. But for now there is some setup that can make running and debugging work.

The issue is we want to run with the `moja.cli.exe` from the moja.FLINT project, but debug in our current IDE (FLINT.example).

The solution is to use properties to setup a Debug run in the IDE, making the command run `moja.cli.exe`.

![VS2019_Debugsetup2](Documentation/VS2019_Debugsetup2.png)

The settings are:

```
# Command
C:\Development\moja-global\FLINT\Source\build\bin\Debug\moja.cli.exe

# Command Args
--config config\point_example.json --logging_config config\logging.debug_on.conf
 
# Working Directory
..\..\..\Run_Env

# Environment
PATH=C:\Development\moja-global\vcpkg\installed\x64-windows\debug\bin;C:\Development\moja-global\FLINT\Source\build\bin\Debug;%PATH%
LOCAL_LIBS=$(OutDir)
```

With Envs: `PATH` for various libraries built in the Moja stage and `LOCAL_LIBS` so we can modify the explicit path for our example config to load libraries (the default is the same location as the EXE).

To match this, the example point config uses an environment variable in the library path:

```json
  "Libraries": {
    "moja.flint.example.base": {
      "library": "moja.flint.example.based.dll",
      "path": "%LOCAL_LIBS%",
      "pathEXPLICIT": "C:/Development/moja-global/FLINT.example/Source/build/bin/Debug",
      "type": "external"
    }
  },
```



