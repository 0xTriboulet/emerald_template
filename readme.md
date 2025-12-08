# emerald-template

**emerald-template** is a CMake-based project template designed for developing and debugging Reflective DLL Loaders using the **Crystal Palace** linker.

This template wraps the functionality of the "Simple Loader 1" example provided by the Tradecraft Garden. It transforms the standard makefile-based workflow into a **CLion-compatible** project. This allows for source-code level debugging of your loader logic from **Windows** (and theoretically Linux) systems before compiling the final position-independent shellcode.

This project follows the design patterns established by [BOF-VS](https://github.com/Cobalt-Strike/bof-vs). While the loader is fundamentally C code, this template compiles it using a C++ compiler with `extern "C"` blocks. This enables the use of **DFR (Dynamic Function Resolution)** macros and other modern conveniences while maintaining the structure required for reflective loading.

## Prerequisites

To use this template effectively, you require the following tools:

*   **CLion**: [Download Here](https://www.jetbrains.com/clion/download/?section=windows). This is the primary IDE for this template.
*   **WSL (Windows Subsystem for Linux)**: [Video Guide](https://tradecraftgarden.org/videos.html). You must have a WSL distribution installed (e.g., Ubuntu).
    *   Ensure `make` and `mingw-w64` are installed inside your WSL instance (`sudo apt install make mingw-w64`).
*   **MinGW Binaries for Windows**: [Download Here](https://github.com/niXman/mingw-builds-binaries/releases). These binaries must be installed on your Windows host and added to your System PATH to allow CLion to compile the debug executable locally.
*   **Python 3**: Required for the helper script that converts your target DLL into a C header for debugging.

## Setup

**Important:** Crystal Palace is not released on public git platforms. Consequently, it cannot be included as a submodule in this repository. You must download the necessary files and arrange them manually.

1.  Download **Crystal Palace**: [https://tradecraftgarden.org/crystalpalace.html](https://tradecraftgarden.org/crystalpalace.html)
2.  Download **Tradecraft Garden** source (for libtcg and examples): [https://tradecraftgarden.org/tradecraft.html](https://tradecraftgarden.org/tradecraft.html)

### Folder Structure

Organize your project folder to match the following structure exactly. You will need to copy the `crystal-palace` binary folder and the `tcg` source folder into the project root.

```plain text
emerald-template>tree
Folder PATH listing for volume Windows
Volume serial number is 5048-32BA
C:.
├───.idea
├───bin
├───crystal-palace
│   └───demo
│       └───src
├───debug-dependencies
│   └───bin
├───tcg
     ├───libtcg
     │   ├───bin
     │   └───src
   ...snip...
     └───simple_rdll_patch
        └───src
```


## Usage

This template is designed to handle two distinct workflows: **Debugging** (creating an EXE) and **Building** (creating the reflective loader object).

### Configuration

The target DLL and build specification are controlled in `CMakeLists.txt`.

```cmake
set(LOADER_SPEC "loader.spec")
set(DLL_PATH "crystal-palace/demo/test.${ARCH_EXT}.dll")
```


By default, the template uses the `test.*.dll` found in Crystal Palace's demo folder.

### 1. Debugging (Debug Build)

When you select the **Debug** profile in CLion:

1.  CMake checks for `debug.h`.
2.  If missing, it executes `loader.py`. This script reads the DLL specified in `DLL_PATH` and converts it into a C byte array inside `debug.h`.
3.  The project is compiled as a standard Windows Executable (`.exe`).
4.  The entry point is hijacked to `WinMain`, allowing you to step through `go()` and other loader functions using the CLion debugger.
5.  External function resolution (e.g., `VirtualAlloc`) is macro-switched to use local imports instead of the hashed resolution used in the final shellcode.

**Changing the Target DLL:**
If you change the `DLL_PATH` in CMake or modify the target DLL, you must manually delete `debug.h` or re-run `loader.py` to ensure the bytes in the header match your new DLL.

6. CLion should automati
```shell script
python loader.py path/to/your/new.dll
```


### 2. Building (Release Build)

When you select the **Release** profile in CLion:

1.  The code is compiled into an object file (`loader.x64.o` or `loader.x86.o`).
2.  Optimization flags (`-O0`) are applied (or disabled) based on your configuration.
3.  CMake invokes **WSL**.
4.  Inside WSL, it runs the **Crystal Palace Linker** (`crystal-palace/link`) using the `loader.spec` file.
5.  The final position-independent loader object is output to the `bin/` directory.



### Adding Your Code
For now, additionall .cpp files should __*not*__ be added to `CMakeLists.txt`. Instead, they should be added to the `src/` directory and then included in `main.cpp` using `#include "your_file.cpp"`.

## Customizing loader.spec

The `loader.spec` file controls how Crystal Palace links your object files. Ensure the paths in this file match your project structure.

If you wish to pass the DLL path dynamically during the Release build, ensure your `loader.spec` relative paths to your DLL. Using a different DLL requires updating the paths in `loader.spec`. This is a limitation of passing environment variables to WSL that I could not figure out how to overcome.

## Credits

*   **Raphael Mudge**: For Crystal Palace and the Tradecraft Garden examples.
*   **Cobalt Strike / Fortra**: For the BOF-VS and UDRL-VS pattern that inspired this template's structure.