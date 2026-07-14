# Deterministic simulation contract

## Math

The authoritative math and physics implementation lives in the pure C11 `kinefix` dependency. Arena3D pins the dependency by commit and exposes only a thin C++ adapter to the rest of the example.

`Fixed` is signed Q32.32 stored in `int64_t`. Decimal parsing, multiplication, and division truncate toward zero. Kinefix uses `__int128` with Clang and a portable 64-bit limb backend with MSVC so both platforms execute the same integer operations. Division by zero and overflow assert in debug and set a serialized `SimulationFault` in release, stopping the tick.

Vector dot/cross, integer square root, normalization, and binary-angle sine/cosine do not call the platform floating-point math library. One yaw turn maps to all 65536 values of `uint16_t`; signed `int16_t` stores pitch and look deltas. Sine and cosine use a Q32.32 quarter-wave table with integer interpolation, and cosine is a quarter-turn phase shift. Presentation is allowed to convert copied angles and state to float; authoritative code is not.

Parametric queries use a normalized `[0, 1]` fraction and test the segment endpoints before division, so very small non-zero displacements cannot overflow a slab calculation and are not discarded by a timing epsilon. World queries select the smallest fraction and then the smallest object ID. `kf_hit_t` also returns distance, position, normal, and whether the query began inside the collider.

Ground slide thresholds, friction, acceleration scale, and speed cap are compiled from decimal content values. A slide is derived only from authoritative crouch, grounded, and horizontal velocity state, so it needs no presentation-side timer.

Kinefix also owns authoritative Y-up capsule character integration, support and step resolution, AABB/sphere/capsule overlaps, raycasts, sphere/AABB and sphere/capsule casts, and capsule/AABB sweeps. Character and projectile motion is continuous over the complete tick displacement, preventing tunneling through thin brushes at high speed. Capsule/AABB casts conservatively expand the brush by the radius at corners: an early contact is allowed, passing through a wall is not. Arena3D keeps gameplay decisions such as acceleration inputs, weapon rules, damage, impulses, and jump-pad effects in the simulation layer.

## Identity and ordering

- Gameplay IDs are `uint32_t`; projectile IDs come from a monotonic counter and never use pool indices.
- Commands for a tick are stable-sorted by player ID and sequence. Duplicates, invalid button/fire transitions, wrong match/player IDs, and non-admin commands during pause are journalled but rejected.
- Source time is mapped with `ceil(sourceTimeMs / 10)`. Late commands are moved to the first unprocessed tick and retain the correction bit.
- Damage is stable-sorted by target, source, and creation order before application.
- Events are stable-sorted by type, actor, target, weapon, position, and amount. `EventId` is `(tick << 32) | ordinal` after sorting.
- Collision brushes, uniform-grid cells, cell brush lists, Railgun hits, projectiles, turrets, and pickups are traversed in stable ID/slot order.

PCG32 uses separate weapon, per-turret aim, and pickup streams. Pellet spread is reconstructed from match seed and stable shot ID, so adding an unrelated random call cannot change a shotgun pattern.

## Serialization and versions

Canonical serialization writes explicit little-endian fields without native structs or padding. The protocol tracks simulation, deterministic math, command, snapshot, config, collision, and replay versions separately.

Snapshots contain a versioned header, payload length, and CRC. Replays contain content CRCs, canonical command envelopes, authoritative events, optional snapshot checkpoints, and a final subsystem/full checksum. Render frames are never recorded.

After a replay failure, the tool restores the expected checkpoint and reports the first differing authoritative object/field. Example:

```sh
Arena3DReplayTool match.a3r --inject-fault 100
```

The command succeeds only when the injected fault is detected.

## Required gates

For release candidates run:

```sh
ctest --test-dir examples/arena3d/solutions/headless --output-on-failure
examples/arena3d/solutions/headless/tools/replay/Arena3DReplayTool --soak 60000
examples/arena3d/solutions/headless/tools/replay/Arena3DReplayTool --soak 180000
```

The first soak is ten game minutes at 100 Hz. The second is thirty minutes and includes all ten turrets, a deterministic projectile stress profile that must exceed 100 concurrent projectiles, dual-instance per-tick CRC comparison, and 20 restart cycles.
