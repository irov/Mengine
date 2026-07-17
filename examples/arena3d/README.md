# Arena3D

Arena3D is a self-contained Mengine 3D arena example. Gameplay runs in a 100 Hz fixed-point simulation which has no dependency on Mengine scene, rendering, sound, wall-clock time, or render FPS. Deterministic math and physics come from the pure C `kinefix` dependency. The Mengine framework can use either the in-process deterministic session or the same UDP client used by standalone bots against a dedicated authoritative server.

The example targets macOS/Metal and Windows x64/DX11. Arena presentation and authoritative collision are generated as separate assets: collision stays a compact deterministic brush set, while the render arena uses denser chamfered geometry, twelve material chunks, world-scaled UVs, per-corner vertex colors, and independent stone, metal, emissive and hazard textures. Low-poly weapons, articulated turret modules, projectiles, pickups, textures, and WAV effects use the same procedural pipeline; Blender and external asset packs are not required.

Arena01 is an original multi-level layout inspired by the flow of classic arena shooters: a central atrium with twin stairs, asymmetric rail/armor balconies, a rocket bridge, three jump-pad routes, a damaging lava basin, and a real bottomless fall route backed by an authoritative kill volume. It does not contain copied Quake geometry, textures, or sounds.

## Quick start

Headless core and replay tool:

```sh
examples/arena3d/build/headless/build_headless.sh
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

To generate and immediately build the macOS application, dedicated server, bot, and proxy:

```sh
examples/arena3d/build/xcode_macos/build_solution_xcode_macos_debug.sh
open bin/Mengine_Xcode_MacOS/Xcode/Debug/MacOSApplication.app
```

Run `Win32Application.exe` with `examples/arena3d/resources/Data` as its working directory. The CMake content target generates and validates resources before compiling either application.

The Windows wrappers have matching build entrypoints:

```bat
examples\arena3d\build\msvc17_dx11_x64\build_solution_debug.bat
```

Start a local dedicated match with three independent bot processes:

```sh
python3 examples/arena3d/tools/run_local_match.py --bots 3 --ticks 6000
```

Add an 80 ms round-trip and 2% packet loss through one proxy process per bot:

```sh
python3 examples/arena3d/tools/run_local_match.py --bots 3 --ticks 6000 --latency-ms 40 --loss-permille 20
```

To join the server from the graphical macOS client, start the application executable with Mengine options:

```sh
bin/Mengine_Xcode_MacOS/Xcode/Debug/MacOSApplication.app/Contents/MacOS/MacOSApplication \
  --arena3d-server:127.0.0.1 --arena3d-port:27960 --arena3d-name:human
```

Without `arena3d-server`, the application retains the local session mode.

## Controls

| Input | Action |
|---|---|
| WASD | Move / air-strafe |
| Space | Jump / bunny-hop |
| Left Ctrl | Crouch |
| Mouse | Look |
| Left mouse | Fire / charge |
| 1–6 | Nailgun, Rocket Launcher, Railgun, Plasmagun, Grenade Launcher, Shotgun |
| Q / E | Previous / next weapon with available ammo |
| Esc | Pause and release the mouse / resume and capture it again |
| F1 | Runtime counters and CRC |
| F2 | Collision diagnostic toggle |
| F3 | Turret state diagnostics |
| F4 | Allowed debug noclip |
| F5 | Authoritative match restart |

Movement uses deterministic Quake-style projected acceleration: air-strafing can build speed beyond the base run cap, holding jump preserves momentum across landings, and weapon knockback is added to the current velocity so consecutive rocket, grenade and plasma jumps accumulate speed.

## Runtime architecture

- `kinefix` is a pure C11 library that owns 32-bit Q16.16 values, 64-bit Q32.32 arithmetic intermediates, vectors, `uint16_t` binary angles with lookup-table trigonometry, PCG32, AABB/sphere/capsule collision queries, raycasts, continuous character integration, step handling, and projectile sweeps.
- `Arena3DSimulation` owns canonical game state and rules: commands, weapons, turrets, pickups, damage, events, snapshots, and checksums. Its small C++ fixed-point facade delegates to `kinefix` so existing presentation-facing types remain stable.
- `POSIXSocketSystem` and `Win32SocketSystem` own only native BSD socket/Winsock operations. The legacy TCP `SocketInterface` remains available; `DatagramInterface` adds resolved IPv4/IPv6 addresses, UDP, non-blocking operation, waits, buffer controls, truncation and platform result codes.
- `SocketService` is the platform-neutral reliable-UDP transport. Its network thread owns one endpoint, peer lifecycle, bounded queues, 1200-byte packets, ACK masks, retransmission/backoff, independent channels, fragmentation, keepalive, timeout, bandwidth accounting and statistics. It never invokes application callbacks from its thread.
- `Arena3DNetworking` owns the game wire protocol, compatibility handshake, roster, command batching, compressed authoritative state frames, snapshots, event delivery and reconciliation. The server derives player/match/tick identity from the connected peer and never trusts those client fields.
- `LocalSession` remains available for offline play. Network mode uses the same `NetworkClient` in the Mengine application and in each independent `Arena3DBotClient` process.
- `Arena3DServer`, `Arena3DBotClient`, and `Arena3DNetProxy` share a minimal headless bootstrap with allocator, basic services, time, threads and sockets; they do not link render or audio systems.
- `Arena3DFramework` converts only presentation copies to float. It interpolates camera and actors, pools projectile nodes, red actor-hit flashes, floating health-damage numbers and temporary surface decals, maintains view-model/world camera branches, plays generated audio, deduplicates events, and renders per-weapon icon/ammo HUD slots after post-processing.
- `Arena3DReplayTool` reads `.a3r` command and event journals and verifies content CRCs, checkpoint snapshots, and the final checksum.
- `arena3d_content.py` is the single source-to-assets path. Its `check` command rebuilds into a temporary directory and compares the complete tree digest.

The authoritative order is commands → weapon timers → turret AI → player movement → projectile sweeps → sorted damage/impulses → pickups/death/respawn → events/CRC. Paused simulation time and ticks do not advance; only resume and restart are accepted while frozen.

## Content outputs

Generated files are written to `resources/Data/generated` and intentionally ignored by Git:

- indexed GLB meshes with embedded buffers, positions, hard normals, UV0, vertex colors, and indices;
- independent `stone`, `metal`, and `emissive` arena chunk layers; decorative polygons never enter authoritative collision;
- pooled per-weapon impact flashes and generated scorch decals which follow the authoritative contact point and surface normal, then fade after 12 seconds;
- 64×64 tile, hazard, emissive, and white PNGs;
- generated mono PCM WAV effects;
- canonical config, level, and convex collision binaries with CRCs;
- a generated C++ contract containing the exact Q16.16 config, binary-angle spawn/turret yaw, pickups, jump pads, hazard volumes, collision AABBs, and content CRCs.

See [CONTENT.md](docs/CONTENT.md) to extend arenas or weapons, [DETERMINISM.md](docs/DETERMINISM.md) for the math/replay contract, and [NETWORKING.md](docs/NETWORKING.md) for transport and process usage.

## Build validation

CI checks reproducible generated content, builds the Clang/MSVC headless targets, and builds the complete Metal/DX11 applications. Visual interaction checks remain a platform-runner responsibility because GitHub-hosted jobs do not provide a reliable foreground game window.

## Scope

Encryption, authentication, NAT traversal, matchmaking, bot navmesh, rigid-body middleware, ragdolls, destructible level, skeletal animation, and mobile targets remain outside the example. The current bots are separate network client processes with intentionally simple arena-shooter steering; the server supports up to 16 human/bot peers.
