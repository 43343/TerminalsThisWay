
# Description

The purposes for which you may need TerminalsThisWay:

+ To optimize the work with the terminal
+ You can select any text in your browser, and when you press the keys you specified (Insert by default), the command will be executed automatically in the terminal
+ Also, when you press the keys you selected (Home by default), a window will open for you to select a directory to which you can transfer your command line.

# Install
## Windows
### System requirements

+ Windows 7 or never
+ [Microsoft Visual c++ 2015-2022](https://learn.microsoft.com/ru-ru/cpp/windows/latest-supported-vc-redist?view=msvc-170)

You can download the Windows version below:

+ [64-bit](https://github.com/43343/TerminalsThisWay/releases/download/0.0.2/TerminalsThisWay-0.0.2-win64.exe)
+ ~32-bit~ soon...

## Linux
soon...


# Build
## Windows
### System requirements

+ [Microsoft Visual Studio](https://visualstudio.microsoft.com/ru/downloads/) or [MSYS2/MinGW](https://www.msys2.org/)
+ [CMake](https://cmake.org/)

```bash
git clone https://github.com/43343/TerminalsThisWay.git
cd TerminalsThisWay
mkdir build
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release or cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

For the convenience of compiling c using MinGW via Linux, you can use a toolchain file for convenience. Example:
```bash
-DCMAKE_TOOLCHAIN_FILE=mingw-linux-toolchain.cmake DMINGW_TARGET_ARCH=x86 or x64
```

## Linux
soon...
