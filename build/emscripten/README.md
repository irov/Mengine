# Emscripten build

## Prerequisites

Fetch the dependencies, including the emsdk checkout, then install and activate the pinned Emscripten 5.0.7:

```bash
bash build/downloads/downloads_unix.bash
dependencies/emsdk/emsdk install 5.0.7
dependencies/emsdk/emsdk activate 5.0.7
```

## Build

Run the scripts from any working directory:

```bash
bash build/emscripten/build_depends_emscripten_debug.sh
bash build/emscripten/build_solution_emscripten_debug.sh
```

Use the matching `*_release.sh` wrappers for a Release build, and the `.bat` scripts on Windows (they use the Ninja generator).

`MENGINE_PLUGIN_ASTRALAX` becomes available when the private package is placed in `dependencies/astralax_emscripten` (`include/magic.h`, `lib/libastralax.a`); a project enables it from its `mengine_prepare.cmake`.

The engine solution builds the runtime without project content. A project passes its deploy folder through `MENGINE_DEPLOY_PATH`; the folder's `project_deploy.cmake` adds the preloaded content and the HTML shell.

## Build options

Set these CMake cache options with `-DNAME=value`; their defaults preserve the current browser configuration.

| Option | Default | Purpose |
| --- | --- | --- |
| `MENGINE_EMSCRIPTEN_ALLOW_MEMORY_GROWTH` | `ON` | Allow memory to grow at runtime. |
| `MENGINE_EMSCRIPTEN_MAXIMUM_MEMORY` | `4GB` | Memory limit when growth is enabled. |
| `MENGINE_EMSCRIPTEN_STACK_SIZE` | `2097152` | Stack size in bytes (2 MiB). |
| `MENGINE_EMSCRIPTEN_INITIAL_MEMORY` | `67108864` | Initial memory in bytes (64 MiB). |
| `MENGINE_EMSCRIPTEN_MIN_WEBGL_VERSION` | `1` | Lowest WebGL version the application may create. |
| `MENGINE_EMSCRIPTEN_MAX_WEBGL_VERSION` | `1` | Highest WebGL version the application tries first. |
| `MENGINE_EMSCRIPTEN_GL_ENABLE_GET_PROC_ADDRESS` | `OFF` | Include OpenGL procedure address lookup. |

For example, `-DMENGINE_EMSCRIPTEN_STACK_SIZE=4194304` selects a 4 MiB stack. A WebGL range of `1`–`2` tries WebGL 2 first and falls back to WebGL 1 if context creation fails; `2`–`2` requires WebGL 2. WebGL 2 uses Emscripten compatibility emulation for the existing GLES 2 renderer. This application target requires the browser environment, a live runtime, and filesystem support.

These options map to the corresponding [Emscripten compiler settings](https://emscripten.org/docs/tools_reference/settings_reference.html). `EmscriptenPersistentUserStorage.js` mounts `/user`, restores its IndexedDB contents before the engine starts, and enables automatic persistence.

## Run

For a project build, serve the generated `Mengine.html` and its data over HTTP with `emrun` or another local HTTP server.

The engine renders into the page element with the id `canvas`. The user folder `/user` is stored in IndexedDB.

## Sound

Streamed sounds (`ResourceMusic` and `ResourceSound` with `IsStreamable`) are played by the browser's own media element, so their files must be in a format every browser decodes: MP3. The resource preparation stage converts them to MP3 with the `mp3Sound` codec, as Mengine-Builder's MP3 sound and music modes do; `MENGINE_PLUGIN_MP3` is enabled by default for the web to read them. Other sounds are decoded by the engine and can stay OGG.
