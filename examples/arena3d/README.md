# Arena3D

Arena3D is a self-contained Mengine 3D arena example. Gameplay runs in a 100 Hz fixed-point simulation which has no dependency on Mengine scene, rendering, sound, wall-clock time, or render FPS. Deterministic math and physics come from the pure C `kinefix` dependency. The Mengine framework can use either the in-process deterministic session or the same UDP client used by standalone bots against a dedicated authoritative server.

The example targets macOS/Metal and Windows x64/DX11. Its arena is generated offline from the downloaded **q3dm6ish_v2** map, a free retexture of aardappel's q3dm6ish by TRaK. The compiled BSP geometry, fourteen TRaK2 textures, original UV sets and 76 baked lightmaps are converted to material-separated GLB meshes and PNG atlases. Mengine samples the original lightmaps per pixel, including their shadows and light pools. The initial spawn overlooks the main hall. The three original jump-pad routes and spawn/item positions are adapted to Arena3D gameplay.

The map and textures are GPL-2.0-or-later content; see [the asset notices](content/third_party/q3dm6ish_v2/README.md), original author readme, license, and download hashes. No Quake III installation, Blender or external Python packages are required. The first CMake configuration downloads assets into the ignored `resources/.downloads` directory and verifies every SHA-256. Subsequent builds use the verified cache offline. Only source manifests and attribution are stored in Git; models, textures, the map, compiled BSP and generated resources must not be committed. Weapons, six ammunition types, the grenade projectile and armor use [OpenArena MD3 models and skins](content/third_party/openarena_models/README.md). The nailgun uses its machinegun model. The example animates weapon recoil, idle/walk motion, muzzle flashes, the separate machinegun barrel, floating/rotating pickups and airborne grenades. Actors, health packs, effects and sky are generated locally. Arbitrary Quake shader scripts and MD3 skeletal/frame animation are not interpreted at runtime.

Rendering preserves the original slanted brush faces. The existing deterministic controller uses AABBs: axial brushes are exact, while bevels and ramps are sliced at quarter-unit intervals. This is a collision approximation, not a Quake BSP implementation. Changing the level also changes content CRCs, so earlier replays are incompatible.

## Quick start

Download or repair the asset cache explicitly (also done by CMake configuration):

```sh
bash examples/arena3d/build/downloads/downloads.sh
```

On Windows run `examples\arena3d\build\downloads\downloads.bat`. Missing or changed files are downloaded to a temporary file and installed only after their hash matches the manifest.

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

## MCP verification

The example's `.mengine/mcp.json` points to the application produced by the existing macOS Debug build scripts. Run the Mengine-MCP server with this descriptor (or open `examples/arena3d` as its workspace), then use `app_launch` with `appId: "arena3d"`, `profileId: "macos-debug"`, and `mode: "hidden_render"`. Keep the client connected while calling `frame_capture`, `logs_read`, and virtual input tools; finish with `app_stop`. This native C++ example does not register a script handler. Use the direct `open` command above for an interactive window that should remain on screen.

The importer checks run with:

```sh
python3 -m unittest discover -s examples/arena3d/tools -p 'test_arena3d_quake_map.py'
python3 examples/arena3d/tools/arena3d_content.py check
```

Movement and weapon regression checks (map stairs up/down and step limits; Railgun click, trail endpoints, replay serialization, damage, wall occlusion, cooldown and optional charge; grenade drop, forward throw, wall bounce, fuse timing and snapshot determinism):

```sh
sh examples/arena3d/build/headless/build_headless.sh Debug
ctest --test-dir examples/arena3d/solutions/headless/Debug --output-on-failure
```

Grenades separate from contacts, damp their bounces, and stop below a speed threshold on floors. Their fuse continues while resting. Railgun fires once on press with a one-second cooldown; a nonzero `charge_ticks` configuration can enable charging. Its blue-white trail stays in world space and fades over 1.5 seconds. The authoritative shot endpoint stops at the first wall and is preserved in network events and replays.

The character steps over ledges up to 18 Quake units (0.5625 at the map's 1:32 scale), including the map's 16-unit stairs. Camera height uses a critically damped response to each rendered rise, preserving smooth upward motion across successive stairs even when several simulation ticks share a frame. Jump and fall movement retain their simulation trajectory. Simulation version 16, replay format 7 and network protocol 3 reject incompatible older recordings and peers.

## Controls

| Input | Action |
|---|---|
| WASD | Move / air-strafe |
| Space | Jump / bunny-hop |
| Left Ctrl | Crouch |
| Mouse | Look |
| Left mouse | Fire |
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
- BSP material chunks with two UV sets; each texture atlas holds a repeating diffuse tile and the corresponding original lightmaps; sky and render-only entities do not enter authoritative collision;
- pooled per-weapon impact flashes and generated scorch decals which follow the authoritative contact point and surface normal, then fade after 12 seconds;
- losslessly converted TRaK2 PNGs at their original sizes, plus procedural actor/effect textures;
- generated mono PCM WAV effects;
- canonical config, level, and convex collision binaries with CRCs;
- generated C++ contracts describing material chunks and the exact Q16.16 config, spawn/turret yaw, pickups, jump pads, hazard volumes, collision AABBs, and content CRCs.

See [CONTENT.md](docs/CONTENT.md) to extend arenas or weapons, [DETERMINISM.md](docs/DETERMINISM.md) for the math/replay contract, and [NETWORKING.md](docs/NETWORKING.md) for transport and process usage.

## Build validation

CI checks reproducible generated content, builds the Clang/MSVC headless targets, and builds the complete Metal/DX11 applications. Visual interaction checks remain a platform-runner responsibility because GitHub-hosted jobs do not provide a reliable foreground game window.

## Scope

Encryption, authentication, NAT traversal, matchmaking, bot navmesh, rigid-body middleware, ragdolls, destructible level, skeletal animation, and mobile targets remain outside the example. The current bots are separate network client processes with intentionally simple arena-shooter steering; the server supports up to 16 human/bot peers.
