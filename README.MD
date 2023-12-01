# cpp-proj

Program to make a cpp project.
Made by Momoyon.

*Only works on Windows as it depends on WinAPI.*

## How it works

Makes a directory named with the project name that you provide and runs `runemacs.exe` on that folder. **Note: if emacs directory must be in your PATH!**
It checks for an environment variable `CppDir` and makes the folder there. If no such environment variable is set, it just uses the current directory.

## Usage

```console
$ Usage: cpp-proj.exe [options] <proj-name>
```

## Building

```console
$ premake5 vs2022
$ msbuild -p:configuration=Debug build\cpp-proj.sln
$ bin\Debug\cpp-proj.exe
```
Note: You can also open the VS solution file (.sln) directly and build with Visual Studio<s>(If you want to wait eternally for it to open)</s> and build it.

## Dependencies
- [premake5 (version 5.0.0-beta2 and up)](https://github.com/premake/premake-core/releases/download/v5.0.0-beta2/premake-5.0.0-beta2-windows.zip)
- [Visual Studio 17.4.4 (2022)](https://visualstudio.microsoft.com/vs/community/) with (Desktop development with C++ Workload Installed)
