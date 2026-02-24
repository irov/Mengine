# Mengine Development Guide

Mengine is a multi-platform C++ game engine built with CMake. See `README.md` for project overview.

## Cursor Cloud specific instructions

### Build environment

The Unix/Linux desktop build requires **clang with libc++** (not GCC's libstdc++) because:
- GCC 13 does not put C99 float math functions (`std::tanf`, `std::sinf`, etc.) into the `std` namespace as required by the engine's `StdMath.h`
- Some third-party dependencies (e.g. metabuf) are missing `<cstdint>` includes required by GCC 13

Required system packages (Ubuntu 24.04):
```
sudo apt-get install -y libssl-dev libgl-dev libglu1-mesa-dev libasound2-dev libpulse-dev \
  libudev-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev libxss-dev \
  libwayland-dev libxkbcommon-dev libxtst-dev libdbus-1-dev libibus-1.0-dev \
  libsamplerate0-dev libpipewire-0.3-dev libjack-dev libsndio-dev libdecor-0-dev \
  libc++-dev libc++abi-dev
```

Additionally, a `libstdc++.so` symlink may need to be created:
```
sudo ln -sf /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /usr/lib/x86_64-linux-gnu/libstdc++.so
```

### Build steps (Unix/Linux Debug)

All build scripts are in `build/unix/` and `build/downloads/`. The three-step process is:

1. **Download dependencies** (clones ~50 git repos, ~2 min):
   ```
   cd build/downloads && bash downloads_unix.bash
   ```

2. **Build dependencies** (compiles all third-party libs, ~2 min):
   ```
   export CC=clang CXX=clang++ CXXFLAGS="-stdlib=libc++" LDFLAGS="-stdlib=libc++"
   cd build/unix && bash build_depends_unix_debug.bash
   ```

3. **Build engine** (requires `-DMENGINE_DEPENDENCIES_PROJECT=Depends_Unix`):
   ```
   export CC=clang CXX=clang++ CXXFLAGS="-stdlib=libc++ -Wno-macro-redefined" CFLAGS="-Wno-macro-redefined" LDFLAGS="-stdlib=libc++"
   mkdir -p solutions/solution_unix/Debug && cd solutions/solution_unix/Debug
   cmake -G "Unix Makefiles" -S "../../../cmake/Unix" -DCMAKE_BUILD_TYPE:STRING=Debug -DMENGINE_DEPENDENCIES_PROJECT=Depends_Unix
   cmake --build ./ -j 8 --config Debug
   ```

### Known Linux build issues

- The `build/unix/build_solution_unix.bash` script does not pass `-DMENGINE_DEPENDENCIES_PROJECT=Depends_Unix`, which is required. Run cmake manually as shown above.
- OpenAL builds as a shared library (`.so`) on Linux but the engine expects a static library (`.a`). Symlink the `.so` to the expected `.a` path.
- The Unix platform build is missing `EnvironmentService` implementation and `CryptographySystem`. All engine libraries compile to 100%, but final linking of `SDL2Application` has remaining unresolved symbols for these missing services.
- The generated `SDL_config.h` must override the source-tree stub. Copy from `outputs/dependencies/config/Depends_Unix/Debug/SDL2/include-config-debug/SDL2/SDL_config.h` to `dependencies/SDL2/include/SDL_config.h`.

### Project structure

- `src/` — Engine source code (Kernel, Engine, Services, Platforms, Plugins, Frameworks)
- `cmake/` — CMake build configs per platform (`Unix/`, `Win32/`, `Android/`, etc.)
- `cmake/Dependencies/` — Third-party library build integration
- `cmake/Downloads/` — Dependency download definitions
- `build/` — Platform-specific build scripts
- `dependencies/` — Downloaded third-party source trees (git-cloned)
- `solutions/` — CMake build output directories
- `outputs/` — Compiled dependency/solution artifacts
- `examples/` — Example game projects (bubblegum, tictactoe, etc.)

### No automated tests

This project does not have automated test suites. Verification is done through building and running the engine.

### No linter configuration

There is no ESLint, clang-tidy, or other linter configuration in the repository. Code quality is checked via CodeFactor, Codacy, and CodeQL (see `.github/workflows/codeql-analysis.yml`).
