# AGENTS.md

## Cursor Cloud specific instructions

### Overview

Mengine is a multi-platform C++ game engine using CMake as its build system. The primary development targets are Windows (MSVC) and macOS (Xcode). The Linux/Unix build path exists but is incomplete — see known issues below.

### Build process (Linux)

The standard build steps are documented in the README (Windows) and `build/unix/README.md`. On Linux, the workflow is:

1. **Install system deps**: `sudo apt install libssl-dev libgl-dev libasound2-dev libpulse-dev libudev-dev libsdl2-dev libxi-dev libxext-dev libxrandr-dev libxcursor-dev libxss-dev libxxf86vm-dev libxinerama-dev libwayland-dev libxkbcommon-dev mesa-common-dev libxtst-dev libstdc++-13-dev`
2. **Download dependencies**: `cd build/downloads && bash downloads_unix.bash` (clones ~60 third-party repos via CMake; takes ~2 minutes)
3. **Build dependencies**: Custom cmake invocation from `build/unix/` (see caveats below)
4. **Build solution**: Custom cmake invocation (see caveats below)

### Critical Linux build caveats

- **Clang linker fix**: The default Clang on Ubuntu 24.04 can't find `libstdc++.so`. Fix: `sudo ln -sf /usr/lib/gcc/x86_64-linux-gnu/13/libstdc++.so /usr/lib/x86_64-linux-gnu/libstdc++.so`
- **SDL3 must be disabled**: The SDL3 dependency build breaks OpenAL-Soft's `find_package(SDL3)`. Pass `-DMENGINE_DEPENDENCE_SDL3=OFF` when configuring dependencies. Also disable Windows-only deps: `-DMENGINE_DEPENDENCE_GLAD=OFF -DMENGINE_DEPENDENCE_GLFW3=OFF -DMENGINE_DEPENDENCE_OPTICK=OFF -DMENGINE_DEPENDENCE_RENDERDOC=OFF -DMENGINE_DEPENDENCE_OPENSSL=OFF`
- **Compatibility header required**: The codebase uses MSVC-specific C++ extensions (`std::tanf`, `std::sinf`, etc.) and missing Linux defines (`MENGINE_MALLOC_SIZE`, `localtime_s`). A compatibility header at `solutions/mengine_compat.h` provides these via `-include` flag.
- **SDL2 config copy**: After building dependencies, copy the generated SDL2 config: `cp outputs/dependencies/config/Depends_Unix/Debug/SDL2/include-config-debug/SDL2/SDL_config.h dependencies/SDL2/include/SDL_config.h`
- **No Unix application entry point**: `src/Applications/SDL2Application/CMakeLists.txt` lacks a `MENGINE_TARGET_UNIX` case in its main file selection. The engine libraries compile but the final executable cannot be linked. A wrapper CMakeLists.txt in `solutions/cmake_unix_lib/` builds the engine without the application.

### Dependency build command (Linux Debug)

```bash
mkdir -p solutions/dependencies_unix/Debug && cd solutions/dependencies_unix/Debug
cmake -G "Unix Makefiles" -S "../../../cmake/Depends_Unix" \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DMENGINE_DEPENDENCE_SDL3=OFF \
  -DMENGINE_DEPENDENCE_GLAD=OFF \
  -DMENGINE_DEPENDENCE_GLFW3=OFF \
  -DMENGINE_DEPENDENCE_OPTICK=OFF \
  -DMENGINE_DEPENDENCE_RENDERDOC=OFF \
  -DMENGINE_DEPENDENCE_OPENSSL=OFF \
  -DCMAKE_CXX_FLAGS="-include /workspace/solutions/mengine_compat.h"
cmake --build ./ -j 8 --config Debug
```

### Solution build command (Linux Debug)

```bash
mkdir -p solutions/solution_unix/Debug && cd solutions/solution_unix/Debug
cmake -G "Unix Makefiles" -S "../../../solutions/cmake_unix_lib" \
  -DCMAKE_BUILD_TYPE:STRING=Debug \
  -DCMAKE_CXX_FLAGS="-include /workspace/solutions/mengine_compat.h" \
  -DCMAKE_C_FLAGS="-include /workspace/solutions/mengine_compat.h" \
  -DMENGINE_DEPENDENCIES_PROJECT=Depends_Unix
cmake --build ./ -j 8 --config Debug
```

### Known Linux build issues (source code gaps)

These are not environment issues — they are genuine missing code in the Linux port:

- `SDL2Input.cpp` missing `#include "Kernel/UnicodeHelper.h"` (fails SDL2Platform build)
- `Bootstrapper.cpp` references `CryptographySystem` and `FileSystem` services with no Unix implementations
- `SDL2Application` has no `UnixMain.cpp` entry point (the `Win32Main.cpp` content would work on Linux since it uses `SDL_main`)

### Static analysis

The project includes a cppcheck pre-commit hook at `.hooks/pre-commit`. To enable it, add to `.git/config`:
```
[include]
    path = ../.gitconfig
```

### No automated test suite

This project has no automated test framework or test targets. Validation is done by building and running the engine with game content.
