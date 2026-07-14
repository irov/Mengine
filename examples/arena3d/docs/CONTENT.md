# Content pipeline

`content/arena3d_source.json` is the editable source of truth. Numeric simulation values are decimal strings; `arena3d_content.py` converts distances and rates directly to Q32.32 by decimal arithmetic and truncation toward zero. Spawn and turret yaw values are converted from decimal radians to canonical `uint16_t` binary turns during the content build.

## Commands

```sh
python3 examples/arena3d/tools/arena3d_content.py build
python3 examples/arena3d/tools/arena3d_content.py validate
python3 examples/arena3d/tools/arena3d_content.py check
```

`build` recreates `resources/Data/generated`. `validate` checks IDs, archetypes, spawns, brush bounds, binary headers/CRCs, generated headers, and required GLB attributes. `check` performs validation, rebuilds into a temporary directory, and requires an identical tree digest.

## Adding arena geometry

Edit `arena_mesh()` and use its `solid()` helper for authoritative static geometry. The helper emits both visible triangles and a convex AABB brush, so render and physics cannot silently drift. Pass `collide=False` only for explicitly decorative geometry. Brushes receive monotonic IDs and are emitted as fixed-point planes/AABBs plus sorted 8 m uniform-grid cells.

Keep player spawns above non-solid ground, within the level bounds, and outside every authoritative brush when tested with the configured player capsule. Add stable numeric IDs for every spawn, turret, pickup, and trigger. Duplicate IDs fail validation.

## Adding a weapon

1. Add the archetype and all decimal/integer parameters in `arena3d_source.json`.
2. Extend `WEAPON_NAMES`, `weapon_mesh`, turret module generation, projectile generation, and audio generation.
3. Add the enum value and authoritative behavior in `Arena3DSimulation` without using render state or floating-point math.
4. Add Metal and DX11 presentation material mapping if a new visual path is needed.
5. Add a replay test with a stable shot ID, projectile count/pattern, hit result, and final CRC.

The generated C++ contract is compiled into the simulation, so defaults, collision, spawns, turrets, pickups, and replay content CRCs all come from the same build output.

## Asset constraints

GLBs contain exactly one indexed triangle primitive with an embedded buffer and `POSITION`, `NORMAL`, `TEXCOORD_0`, and `COLOR_0`. Materials and textures are assigned by the application. Turrets are separate base/yaw and barrel/pitch nodes; no skeleton is required. PNG and WAV writers use only the Python standard library, and generated game assets carry no third-party pack dependency.
