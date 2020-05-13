# FLINT.example
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors)

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

### Test Module Example

The settings required in VS2019 are:

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

#### RothC example

There is also a RothC example, to run that project use the smae setup as below but change the command arguments:

```powershell
# Command Args
--config config\point_rothc_example.json --logging_config config\logging.debug_on.conf
```



## How to Get Involved?  

moja global welcomes a wide range of contributions as explained in [Contributing document](https://github.com/moja-global/About-moja-global/blob/master/CONTRIBUTING.md) and in the [About moja-global Wiki](https://github.com/moja-global/.github/wiki).  


## FAQ and Other Questions  

* You can find FAQs on the [Wiki](https://github.com/moja.global/.github/wiki).  
* If you have a question about the code, submit [user feedback](https://github.com/moja-global/About-moja-global/blob/master/Contributing/How-to-Provide-User-Feedback.md) in the relevant repository  
* If you have a general question about a project or repository or moja global, [join moja global](https://github.com/moja-global/About-moja-global/blob/master/Contributing/How-to-Join-moja-global.md) and 
    * [submit a discussion](https://help.github.com/en/articles/about-team-discussions) to the project, repository or moja global [team](https://github.com/orgs/moja-global/teams)
    * [submit a message](https://get.slack.help/hc/en-us/categories/200111606#send-messages) to the relevant channel on [moja global's Slack workspace](mojaglobal.slack.com). 
* If you have other questions, please write to info@moja.global   
  

## Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="http://moja.global"><img src="https://avatars1.githubusercontent.com/u/19564969?v=4" width="100px;" alt=""/><br /><sub><b>moja global</b></sub></a><br /><a href="#projectManagement-moja-global" title="Project Management">📆</a></td>
    <td align="center"><a href="https://github.com/leitchy"><img src="https://avatars0.githubusercontent.com/u/3417817?v=4" width="100px;" alt=""/><br /><sub><b>James Leitch</b></sub></a><br /><a href="https://github.com/moja-global/FLINT.example/commits?author=leitchy" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!


## Maintainers Reviewers Ambassadors Coaches

The following people are Maintainers Reviewers Ambassadors or Coaches  

<table><tr><td align="center"><a href="http://moja.global"><img src="https://avatars1.githubusercontent.com/u/19564969?v=4" width="100px;" alt="moja global"/><br /><sub><b>moja global</b></sub></a><br /><a href="#projectManagement-moja-global" title="Project Management">📆</a></td></tr></table>  

**Maintainers** review and accept proposed changes  
**Reviewers** check proposed changes before they go to the Maintainers  
**Ambassadors** are available to provide training related to this repository  
**Coaches** are available to provide information to new contributors to this repository  
