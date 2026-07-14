# Arena3D

Arena3D is a self-contained Mengine 3D arena example. Gameplay runs in a 100 Hz fixed-point simulation which has no dependency on Mengine scene, rendering, sound, wall-clock time, or render FPS. Deterministic math and physics come from the pure C `kinefix` dependency. The Mengine framework is a presentation/client adapter around a separate local authoritative server.

The example targets macOS/Metal and Windows x64/DX11. All arena geometry, collision, low-poly weapons, articulated turret modules, projectiles, pickups, textures, and WAV effects are generated from `content/arena3d_source.json`; Blender and external asset packs are not required.

## Quick start

Headless core and replay tool:

```sh
examples/arena3d/build/headless/build_headless.sh
examples/arena3d/solutions/headless/Debug/tools/replay/Arena3DReplayTool --soak 60000
```

Generate the standard macOS Xcode solution (after `Depends_Xcode_MacOS` has been built):

```sh
examples/arena3d/build/xcode_macos/make_solution_xcode_macos_debug.sh
open examples/arena3d/solutions/solution_xcode_macos/Debug/Arena3D_Xcode_MacOS.xcodeproj
```

Generate the Windows x64/DX11 Visual Studio 2022 solution:

```bat
examples\arena3d\build\msvc17_dx11_x64\make_solution_debug.bat
```

The generated solution is written to `examples/arena3d/solutions/solution_msvc17_dx11_x64/Debug`. Visual Studio 2026 wrappers are also available under `build/msvc18_dx11_x64`.

To generate and immediately build the macOS application:

```sh
examples/arena3d/build/xcode_macos/build_solution_xcode_macos_debug.sh
open bin/Mengine_Xcode_MacOS/Xcode/Debug/MacOSApplication.app
```

Run `Win32Application.exe` with `examples/arena3d/resources/Data` as its working directory. The CMake content target generates and validates resources before compiling either application.

## Controls

| Input | Action |
|---|---|
| WASD | Move / air-strafe |
| Space | Jump / bunny-hop |
| Left Ctrl | Crouch |
| Mouse | Look |
| Left mouse | Fire / charge |
| 1–6 | Nailgun, Rocket Launcher, Railgun, Plasmagun, Grenade Launcher, Shotgun |
| Esc | Pause / resume at a tick boundary |
| F1 | Runtime counters and CRC |
| F2 | Collision diagnostic toggle |
| F3 | Turret state diagnostics |
| F4 | Allowed debug noclip |
| F5 | Authoritative match restart |

## Runtime architecture

- `kinefix` is a pure C11 library that owns Q32.32 math, vectors, `uint16_t` binary angles with lookup-table trigonometry, PCG32, AABB/sphere/capsule collision queries, raycasts, continuous character integration, step handling, and projectile sweeps.
- `Arena3DSimulation` owns canonical game state and rules: commands, weapons, turrets, pickups, damage, events, snapshots, and checksums. Its small C++ fixed-point facade delegates to `kinefix` so existing presentation-facing types remain stable.
- `LocalSession` owns separate server and client simulations connected through `ICommandTransport`. The server schedules and canonicalizes commands, publishes acknowledgements/events/CRC/snapshots, and the client reconciles on mismatch.
- `Arena3DFramework` converts only presentation copies to float. It interpolates camera and actors, pools projectile nodes, maintains view-model/world camera branches, plays generated audio, deduplicates events, and renders HUD after post-processing.
- `Arena3DReplayTool` verifies `.a3r` command and event journals, content CRCs, checkpoint snapshots, and the final checksum. `--inject-fault <tick>` verifies desync detection; `--soak <ticks>` runs two independent simulations plus 20 restarts.
- `arena3d_content.py` is the single source-to-assets path. Its `check` command rebuilds into a temporary directory and compares the complete tree digest.

The authoritative order is commands → weapon timers → turret AI → player movement → projectile sweeps → sorted damage/impulses → pickups/death/respawn → events/CRC. Paused simulation time and ticks do not advance; only resume and restart are accepted while frozen.

## Content outputs

Generated files are written to `resources/Data/generated` and intentionally ignored by Git:

- indexed GLB meshes with embedded buffers, positions, hard normals, UV0, vertex colors, and indices;
- 64×64 tile, hazard, emissive, and white PNGs;
- generated mono PCM WAV effects;
- canonical config, level, and convex collision binaries with CRCs;
- a generated C++ contract containing the exact Q32.32 config, binary-angle spawn/turret yaw, pickups, collision AABBs, and content CRCs.

See [CONTENT.md](docs/CONTENT.md) to extend arenas or weapons and [DETERMINISM.md](docs/DETERMINISM.md) for the math/replay contract.

## Verification

The headless test suite covers fixed-point golden values and negative arithmetic, trigonometry, serializer layout, PCG parity, snapshots, reconciliation, command correction and rejection, pause/restart, capsule contacts and stepping, deterministic ground sliding, high-speed tunneling prevention, continuous projectile sweep, stable collision tie-breaking, every weapon archetype, cold nail accuracy, the 150-tick Railgun, shotgun pattern stability, replay verification, fault injection, and 30/60/144/240 render scheduling parity.

CI runs content reproducibility, Clang/MSVC headless tests, a deterministic soak, and complete Metal/DX11 application builds. Visual interaction smoke checks remain a platform-runner responsibility because GitHub-hosted jobs do not provide a reliable foreground game window.

## Scope

Arena3D deliberately has no network transport, dedicated server process, bots/navmesh, rigid-body middleware, ragdolls, destructible level, skeletal animation, mobile target, or changes to public Mengine C++ interfaces. The local transport is the seam for a future network implementation.
