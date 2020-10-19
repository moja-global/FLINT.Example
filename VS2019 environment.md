# Steps to create a development environment for the Flint.

## For Windows  x64

Prerequest: Visual Studio 2019, CMake

### Step's -  
#### create a working directory:
```powershell
mkdir C:\Development
cd C:\Development
mkdir moja-global
cd moja-global
```
#### moja-global vcpkg :     
```powershell
git clone https://github.com/moja-global/vcpkg
cd vcpkg 
.\bootstrap-vcpkg
vcpkg.exe install boost-test:x64-windows boost-program-options:x64-windows boost-log:x64-windows turtle:x64-windows zipper:x64-windows poco:x64-windows libpq:x64-windows gdal:x64-windows sqlite3:x64-windows boost-ublas:x64-windows fmt:x64-windows 
```
#### FLINT configuration:  
```powershell
cd C:\Development\moja-global
git clone https://github.com/moja-global/FLINT.git
cd FLINT\Source
git checkout develop
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=C:\Development\Software\moja -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_TESTS=OFF -DENABLE_MOJA_MODULES_ZIPPER=OFF -DCMAKE_TOOLCHAIN_FILE=C:\Development\moja-global\vcpkg\scripts\buildsystems\vcpkg.cmake ..
```
Open the visual studio solution that CMake created at `C:\Development\moja-global\FLINT\Source\build\moja.sln`

build the debug configuration **ALL_BUILD** target by right clicking the **ALL_BUILD** node and selecting Build.

![VS2019_Debugsetup](Documentation/VS2019_buildall.JPG)

#### FLINT.example configuration:

```powershell
cd C:\Development\moja-global
git clone https://github.com/moja-global/FLINT.example.git
cd C:\Development\moja-global\FLINT.example\Source
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=C:\Development\Software\moja -DOPENSSL_ROOT_DIR=c:\Development\moja-global\vcpkg\installed\x64-windows -DVCPKG_TARGET_TRIPLET=x64-windows -DENABLE_TESTS=OFF -DCMAKE_TOOLCHAIN_FILE=C:\Development\moja-global\vcpkg\scripts\buildsystems\vcpkg.cmake ..
```

Open the visual studio solution that CMake created at ``C:\Development\moja-global\FLINT.Example\Source\build\flint.example.sln``

Build the debug configuration **ALL_BUILD** target by right clicking the **ALL_BUILD** node and selecting Build. Then right click the the **moja.flint.example.rothc** node and select **Set as Startup Project** then right click again and select properties. Navigate to Configuration Properties/Debugging properties pane and configure the following:

- Command: ``C:\Development\moja-global\FLINT\Source\build\bin\Debug\moja.cli.exe``

- Command Arguments: ``--config config\point_example.json --config config\debug\libs.base.win.json  --logging_config logging.debug_on.conf``
- Working Directory: ``$(SolutionDir)..\..\Run_Env``
- Environment: ``PATH=C:\Development\moja-global\FLINT\Source\build\bin\Debug;%PATH%``
  ``LOCAL_LIBS=C:\Development\moja-global\FLINT.Example\Source\build\bin\Debug``

![VS2019_Debugsetup](Documentation/VS2019_rothcproperties.JPG)

#### Running the RothC example:

You should now be able to select Debug->Start Debugging to start a debug run of the RothC example. You should see something like the following:

![VS2019_Debugsetup](Documentation/VS2019_debugrothc.JPG)

### References : 
https://github.com/moja-global/FLINT/blob/master/README.md

https://github.com/moja-global/FLINT.example/blob/master/README.md



 
