#!/usr/bin/env python3
"""Deterministic, dependency-free Arena3D content builder.

The source JSON stores all simulation decimals as strings.  This tool converts
them directly to Q32.32 and creates the low-poly GLB, PNG, WAV and canonical
binary files consumed by the example.  No Blender installation or downloaded
gameplay assets are required.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import shutil
import struct
import tempfile
import wave
import zlib
from dataclasses import dataclass, field
from decimal import Decimal, InvalidOperation, ROUND_DOWN
from pathlib import Path
from typing import Iterable, Sequence


FORMAT_VERSION = 1
LEVEL_FORMAT_VERSION = 2
Q32_SCALE = 1 << 32
ANGLE16_TURN = 1 << 16
FIXED_TWO_PI = 26986075409
ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT / "content" / "arena3d_source.json"
DEFAULT_OUTPUT = ROOT / "resources" / "Data" / "generated"
WEAPON_NAMES = ("nailgun", "rocket", "railgun", "plasmagun", "grenade", "shotgun")


def fixed(value: str) -> int:
    try:
        raw = (Decimal(value) * Q32_SCALE).to_integral_value(rounding=ROUND_DOWN)
    except InvalidOperation as exc:
        raise ValueError(f"invalid fixed decimal {value!r}") from exc
    result = int(raw)
    if result < -(1 << 63) or result > (1 << 63) - 1:
        raise ValueError(f"fixed decimal out of range: {value}")
    return result


def fixed_angle(value: str) -> int:
    raw = fixed(value) % FIXED_TWO_PI
    numerator = raw * ANGLE16_TURN
    ticks = (numerator + FIXED_TWO_PI // 2) // FIXED_TWO_PI
    return ticks & 0xFFFF


def align4(data: bytearray, padding: int = 0) -> None:
    while len(data) & 3:
        data.append(padding)


@dataclass
class Mesh:
    positions: list[tuple[float, float, float]] = field(default_factory=list)
    normals: list[tuple[float, float, float]] = field(default_factory=list)
    colors: list[tuple[float, float, float, float]] = field(default_factory=list)
    uvs: list[tuple[float, float]] = field(default_factory=list)
    indices: list[int] = field(default_factory=list)

    def vertex(self, position: Sequence[float], normal: Sequence[float], color: Sequence[float], uv: Sequence[float]) -> int:
        index = len(self.positions)
        self.positions.append(tuple(float(v) for v in position))
        self.normals.append(tuple(float(v) for v in normal))
        self.colors.append(tuple(float(v) for v in color))
        self.uvs.append(tuple(float(v) for v in uv))
        return index


def add_quad(mesh: Mesh, points: Sequence[Sequence[float]], normal: Sequence[float], color: Sequence[float], uv_scale: float = 1.0) -> None:
    base = len(mesh.positions)
    uvs = ((0.0, 0.0), (uv_scale, 0.0), (uv_scale, uv_scale), (0.0, uv_scale))
    for point, uv in zip(points, uvs):
        mesh.vertex(point, normal, color, uv)
    mesh.indices.extend((base, base + 1, base + 2, base, base + 2, base + 3))


def add_box(mesh: Mesh, center: Sequence[float], size: Sequence[float], color: Sequence[float], uv_scale: float = 1.0) -> None:
    cx, cy, cz = center
    hx, hy, hz = (size[0] * 0.5, size[1] * 0.5, size[2] * 0.5)
    faces = (
        (((cx - hx, cy - hy, cz - hz), (cx - hx, cy + hy, cz - hz), (cx + hx, cy + hy, cz - hz), (cx + hx, cy - hy, cz - hz)), (0, 0, -1)),
        (((cx + hx, cy - hy, cz + hz), (cx + hx, cy + hy, cz + hz), (cx - hx, cy + hy, cz + hz), (cx - hx, cy - hy, cz + hz)), (0, 0, 1)),
        (((cx - hx, cy - hy, cz + hz), (cx - hx, cy + hy, cz + hz), (cx - hx, cy + hy, cz - hz), (cx - hx, cy - hy, cz - hz)), (-1, 0, 0)),
        (((cx + hx, cy - hy, cz - hz), (cx + hx, cy + hy, cz - hz), (cx + hx, cy + hy, cz + hz), (cx + hx, cy - hy, cz + hz)), (1, 0, 0)),
        (((cx - hx, cy + hy, cz - hz), (cx - hx, cy + hy, cz + hz), (cx + hx, cy + hy, cz + hz), (cx + hx, cy + hy, cz - hz)), (0, 1, 0)),
        (((cx - hx, cy - hy, cz + hz), (cx - hx, cy - hy, cz - hz), (cx + hx, cy - hy, cz - hz), (cx + hx, cy - hy, cz + hz)), (0, -1, 0)),
    )
    for points, normal in faces:
        add_quad(mesh, points, normal, color, uv_scale)


def add_cylinder(mesh: Mesh, center: Sequence[float], radius: float, height: float, sides: int, color: Sequence[float]) -> None:
    cx, cy, cz = center
    y0, y1 = cy - height * 0.5, cy + height * 0.5
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        p0 = (cx + math.cos(a0) * radius, y0, cz + math.sin(a0) * radius)
        p1 = (cx + math.cos(a1) * radius, y0, cz + math.sin(a1) * radius)
        p2 = (p1[0], y1, p1[2])
        p3 = (p0[0], y1, p0[2])
        normal = (math.cos((a0 + a1) * 0.5), 0.0, math.sin((a0 + a1) * 0.5))
        add_quad(mesh, (p0, p3, p2, p1), normal, color)
    top = mesh.vertex((cx, y1, cz), (0, 1, 0), color, (0.5, 0.5))
    bottom = mesh.vertex((cx, y0, cz), (0, -1, 0), color, (0.5, 0.5))
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        t0 = mesh.vertex((cx + math.cos(a0) * radius, y1, cz + math.sin(a0) * radius), (0, 1, 0), color, (0, 0))
        t1 = mesh.vertex((cx + math.cos(a1) * radius, y1, cz + math.sin(a1) * radius), (0, 1, 0), color, (1, 0))
        b0 = mesh.vertex((cx + math.cos(a0) * radius, y0, cz + math.sin(a0) * radius), (0, -1, 0), color, (0, 0))
        b1 = mesh.vertex((cx + math.cos(a1) * radius, y0, cz + math.sin(a1) * radius), (0, -1, 0), color, (1, 0))
        mesh.indices.extend((top, t0, t1, bottom, b1, b0))


def write_glb(path: Path, mesh: Mesh) -> None:
    if not mesh.positions or not mesh.indices:
        raise ValueError(f"empty mesh: {path}")
    binary = bytearray()
    views: list[dict[str, int]] = []
    accessors: list[dict[str, object]] = []

    def append(values: Iterable[Sequence[float]], component_count: int, accessor_type: str, target: int) -> int:
        align4(binary)
        offset = len(binary)
        flat: list[float] = []
        rows = list(values)
        for row in rows:
            flat.extend(row)
        binary.extend(struct.pack(f"<{len(flat)}f", *flat))
        view_index = len(views)
        views.append({"buffer": 0, "byteOffset": offset, "byteLength": len(binary) - offset, "target": target})
        accessor: dict[str, object] = {
            "bufferView": view_index,
            "componentType": 5126,
            "count": len(rows),
            "type": accessor_type,
        }
        if accessor_type == "VEC3":
            accessor["min"] = [min(row[i] for row in rows) for i in range(component_count)]
            accessor["max"] = [max(row[i] for row in rows) for i in range(component_count)]
        accessors.append(accessor)
        return len(accessors) - 1

    pos = append(mesh.positions, 3, "VEC3", 34962)
    normal = append(mesh.normals, 3, "VEC3", 34962)
    color = append(mesh.colors, 4, "VEC4", 34962)
    uv = append(mesh.uvs, 2, "VEC2", 34962)
    align4(binary)
    index_offset = len(binary)
    binary.extend(struct.pack(f"<{len(mesh.indices)}I", *mesh.indices))
    index_view = len(views)
    views.append({"buffer": 0, "byteOffset": index_offset, "byteLength": len(binary) - index_offset, "target": 34963})
    accessors.append({"bufferView": index_view, "componentType": 5125, "count": len(mesh.indices), "type": "SCALAR", "min": [min(mesh.indices)], "max": [max(mesh.indices)]})
    index_accessor = len(accessors) - 1

    document = {
        "asset": {"version": "2.0", "generator": "Arena3D deterministic content builder"},
        "buffers": [{"byteLength": len(binary)}],
        "bufferViews": views,
        "accessors": accessors,
        "meshes": [{"name": path.stem, "primitives": [{"attributes": {"POSITION": pos, "NORMAL": normal, "COLOR_0": color, "TEXCOORD_0": uv}, "indices": index_accessor, "mode": 4}]}],
        "nodes": [{"mesh": 0}],
        "scenes": [{"nodes": [0]}],
        "scene": 0,
    }
    json_chunk = bytearray(json.dumps(document, sort_keys=True, separators=(",", ":")).encode("utf-8"))
    align4(json_chunk, 0x20)
    align4(binary)
    total = 12 + 8 + len(json_chunk) + 8 + len(binary)
    payload = bytearray(struct.pack("<4sII", b"glTF", 2, total))
    payload.extend(struct.pack("<I4s", len(json_chunk), b"JSON"))
    payload.extend(json_chunk)
    payload.extend(struct.pack("<I4s", len(binary), b"BIN\0"))
    payload.extend(binary)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(payload)


def write_png(path: Path, width: int, height: int, pixels: bytes) -> None:
    if len(pixels) != width * height * 4:
        raise ValueError("invalid PNG pixel buffer")
    rows = b"".join(b"\x00" + pixels[y * width * 4:(y + 1) * width * 4] for y in range(height))

    def chunk(kind: bytes, data: bytes) -> bytes:
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)

    payload = b"\x89PNG\r\n\x1a\n"
    payload += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
    payload += chunk(b"IDAT", zlib.compress(rows, 9))
    payload += chunk(b"IEND", b"")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(payload)


def write_tone(path: Path, frequency: float, duration: float, decay: float = 4.0) -> None:
    rate = 22050
    frames = int(rate * duration)
    samples = bytearray()
    for index in range(frames):
        t = index / rate
        envelope = max(0.0, 1.0 - t / duration) ** decay
        carrier = math.sin(2.0 * math.pi * frequency * t)
        overtone = 0.3 * math.sin(2.0 * math.pi * frequency * 1.5 * t)
        sample = int(max(-1.0, min(1.0, (carrier + overtone) * envelope * 0.55)) * 32767)
        samples.extend(struct.pack("<h", sample))
    path.parent.mkdir(parents=True, exist_ok=True)
    with wave.open(str(path), "wb") as output:
        output.setnchannels(1)
        output.setsampwidth(2)
        output.setframerate(rate)
        output.writeframes(samples)


def arena_mesh() -> tuple[Mesh, list[tuple[int, tuple[float, float, float], tuple[float, float, float]]], list[Mesh]]:
    mesh = Mesh()
    chunks = [Mesh() for _ in range(4)]
    floor = (0.25, 0.30, 0.36, 1.0)
    wall = (0.42, 0.48, 0.55, 1.0)
    accent = (0.12, 0.65, 0.75, 1.0)
    hazard = (0.95, 0.45, 0.08, 1.0)
    brushes: list[tuple[int, tuple[float, float, float], tuple[float, float, float]]] = []

    def solid(center: tuple[float, float, float], size: tuple[float, float, float], color: tuple[float, float, float, float], collide: bool = True) -> None:
        add_box(mesh, center, size, color, max(size[0], size[2]) / 4.0)
        chunk_index = (1 if center[0] >= 0 else 0) + (2 if center[2] >= 0 else 0)
        add_box(chunks[chunk_index], center, size, color, max(size[0], size[2]) / 4.0)
        if collide:
            brushes.append((len(brushes) + 1, center, size))

    for x in (-16, 16):
        for z in (-16, 16):
            solid((x, -0.5, z), (32, 1, 32), floor)
    for x in (-16, 16):
        solid((x, 3, -32), (32, 6, 1), wall)
        solid((x, 3, 32), (32, 6, 1), wall)
    for z in (-16, 16):
        solid((-32, 3, z), (1, 6, 32), wall)
        solid((32, 3, z), (1, 6, 32), wall)
    for x, z in ((-10, -10), (10, -10), (-10, 10), (10, 10)):
        solid((x, 1.25, z), (4, 2.5, 4), accent)
    solid((-25, 3.5, 0), (10, 1, 22), wall)
    solid((25, 3.5, 0), (10, 1, 22), wall)
    solid((0, 3.5, -25), (22, 1, 10), wall)
    solid((0, 3.5, 25), (22, 1, 10), wall)
    for side in (-1, 1):
        for step in range(8):
            top = 0.25 + step * 0.25
            solid((side * (14 + step * 1.25), top * 0.5, -20), (1.25, top, 5), hazard)
    for z in (-16, 0, 16):
        solid((0, 0.75, z), (2.5, 1.5, 7), wall)
    solid((-22, 0.05, 0), (3, 0.1, 3), (0.05, 1.0, 0.85, 1.0), False)
    return mesh, brushes, chunks


def weapon_mesh(name: str) -> Mesh:
    palette = {
        "nailgun": (0.45, 0.75, 0.95, 1), "rocket": (0.95, 0.35, 0.12, 1),
        "railgun": (0.65, 0.25, 0.95, 1), "plasmagun": (0.10, 0.95, 0.70, 1),
        "grenade": (0.55, 0.75, 0.20, 1), "shotgun": (0.95, 0.70, 0.20, 1),
    }
    mesh = Mesh()
    color = palette[name]
    add_box(mesh, (0, 0, 0), (0.55, 0.38, 1.4), color)
    add_box(mesh, (0, -0.28, 0.2), (0.3, 0.65, 0.35), (0.16, 0.18, 0.22, 1))
    if name in ("rocket", "railgun", "plasmagun"):
        add_cylinder(mesh, (0, 0.08, -0.9), 0.18 if name != "railgun" else 0.11, 0.9, 8, color)
    elif name == "shotgun":
        for x in (-0.13, 0.13):
            add_cylinder(mesh, (x, 0.08, -0.9), 0.09, 0.9, 8, color)
    else:
        add_box(mesh, (0, 0.06, -0.8), (0.18, 0.18, 0.8), color)
    return mesh


def turret_mesh(module: str) -> Mesh:
    mesh = Mesh()
    if module == "base":
        add_cylinder(mesh, (0, 0.35, 0), 0.75, 0.7, 8, (0.22, 0.25, 0.30, 1))
        add_box(mesh, (0, 0.8, 0), (0.8, 0.35, 0.8), (0.35, 0.40, 0.48, 1))
        return mesh
    colors = {name: weapon_mesh(name).colors[0] for name in WEAPON_NAMES}
    add_box(mesh, (0, 0, 0), (0.65, 0.45, 0.65), colors[module])
    add_cylinder(mesh, (0, 0, -0.65), 0.12, 0.9, 8, colors[module])
    return mesh


def projectile_mesh(name: str) -> Mesh:
    mesh = Mesh()
    colors = {
        "nail": (0.7, 0.85, 1, 1), "rocket": (1, 0.35, 0.08, 1),
        "plasma": (0.05, 1, 0.8, 1), "grenade": (0.45, 0.65, 0.18, 1),
        "pellet": (1, 0.85, 0.35, 1),
    }
    if name in ("plasma", "grenade"):
        add_cylinder(mesh, (0, 0, 0), 0.12, 0.24, 8, colors[name])
    else:
        add_box(mesh, (0, 0, 0), (0.08, 0.08, 0.45 if name != "rocket" else 0.7), colors[name])
    return mesh


def box_planes(center: Sequence[float], size: Sequence[float]) -> list[tuple[int, int, int, int]]:
    cx, cy, cz = (fixed(str(v)) for v in center)
    hx, hy, hz = (fixed(str(v / 2.0)) for v in size)
    one = Q32_SCALE
    return [
        (one, 0, 0, cx + hx), (-one, 0, 0, -cx + hx),
        (0, one, 0, cy + hy), (0, -one, 0, -cy + hy),
        (0, 0, one, cz + hz), (0, 0, -one, -cz + hz),
    ]


def write_collision(path: Path, brushes: Sequence[tuple[int, Sequence[float], Sequence[float]]]) -> int:
    payload = bytearray()
    for brush_id, center, size in brushes:
        minimum = tuple(fixed(str(center[i] - size[i] * 0.5)) for i in range(3))
        maximum = tuple(fixed(str(center[i] + size[i] * 0.5)) for i in range(3))
        planes = box_planes(center, size)
        payload.extend(struct.pack("<III6q", brush_id, 1, len(planes), *(minimum + maximum)))
        for plane in planes:
            payload.extend(struct.pack("<4q", *plane))

    cell_size = 8.0
    cells: dict[tuple[int, int], list[int]] = {}
    for brush_id, center, size in brushes:
        minimum_x = math.floor((center[0] - size[0] * 0.5) / cell_size)
        maximum_x = math.floor((center[0] + size[0] * 0.5) / cell_size)
        minimum_z = math.floor((center[2] - size[2] * 0.5) / cell_size)
        maximum_z = math.floor((center[2] + size[2] * 0.5) / cell_size)
        for cell_x in range(minimum_x, maximum_x + 1):
            for cell_z in range(minimum_z, maximum_z + 1):
                cells.setdefault((cell_x, cell_z), []).append(brush_id)
    payload.extend(struct.pack("<qI", fixed(str(cell_size)), len(cells)))
    for (cell_x, cell_z), brush_ids in sorted(cells.items()):
        payload.extend(struct.pack("<iiI", cell_x, cell_z, len(brush_ids)))
        payload.extend(struct.pack(f"<{len(brush_ids)}I", *sorted(brush_ids)))

    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<4sIIII", b"A3CL", FORMAT_VERSION, len(brushes), len(payload), checksum) + payload)
    return checksum


def int64_literal(value: int) -> str:
    return f"-INT64_C({-value})" if value < 0 else f"INT64_C({value})"


def write_collision_header(path: Path, brushes: Sequence[tuple[int, Sequence[float], Sequence[float]]],
                           collision_crc: int, config_crc: int, level_crc: int, source: dict) -> None:
    lines = [
        "#pragma once",
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include \"kinefix/kinefix.h\"",
        "",
        "namespace Arena3DGenerated",
        "{",
        "    using CollisionBoxRaw = kf_aabb_t;",
        "",
        "    struct WeaponConfigRaw",
        "    {",
        "        int32_t damage; uint32_t cooldownTicks; int64_t speed; int64_t gravity; uint32_t lifetimeTicks;",
        "        uint32_t pelletCount; int64_t splashRadius; int64_t knockback; uint32_t ammo; uint32_t chargeTicks;",
        "    };",
        "    struct SpawnRaw { uint32_t id; int64_t position[3]; uint16_t yaw; };",
        "    struct TurretRaw { uint32_t id; uint32_t weapon; int64_t position[3]; uint16_t yaw; };",
        "    struct PickupRaw { uint32_t id; uint32_t type; uint32_t weapon; int64_t position[3]; int32_t amount; };",
        "",
        "    inline constexpr CollisionBoxRaw CollisionBoxes[] =",
        "    {",
    ]
    for brush_id, center, size in brushes:
        minimum = [fixed(str(center[index] - size[index] * 0.5)) for index in range(3)]
        maximum = [fixed(str(center[index] + size[index] * 0.5)) for index in range(3)]
        values = ", ".join(int64_literal(value) for value in minimum + maximum)
        lines.append(f"        {{{brush_id}u, {{{values.split(', ', 3)[0]}, {values.split(', ', 3)[1]}, {values.split(', ', 3)[2]}}}, {{{', '.join(values.split(', ')[3:])}}}}},")
    movement = source["movement"]
    movement_names = {
        "GroundAcceleration": "ground_acceleration", "AirAcceleration": "air_acceleration", "Friction": "friction",
        "Gravity": "gravity", "JumpImpulse": "jump_impulse", "StandingHeight": "standing_height",
        "CrouchedHeight": "crouched_height", "PlayerRadius": "player_radius", "StepHeight": "step_height",
        "MaximumSlopeCosine": "maximum_slope_cosine", "CrouchedSpeedScale": "crouched_speed_scale",
        "SlideMinimumSpeed": "slide_minimum_speed", "SlideFriction": "slide_friction",
        "SlideAccelerationScale": "slide_acceleration_scale", "SlideMaximumSpeed": "slide_maximum_speed",
    }
    lines.extend([
        "    };",
        "    inline constexpr size_t CollisionBoxCount = sizeof(CollisionBoxes) / sizeof(CollisionBoxes[0]);",
    ])
    for cpp_name, json_name in movement_names.items():
        lines.append(f"    inline constexpr int64_t {cpp_name} = {int64_literal(fixed(movement[json_name]))};")
    arena_half_extent = fixed(str(Decimal(source["level"]["size"][0]) * Decimal("0.5")))
    lines.append(f"    inline constexpr int64_t ArenaHalfExtent = {int64_literal(arena_half_extent)};")
    lines.extend(["    inline constexpr WeaponConfigRaw WeaponConfigs[] =", "    {"])
    for name in WEAPON_NAMES:
        weapon = source["weapons"][name]
        values = (
            weapon["damage"], weapon["cooldown_ticks"], int64_literal(fixed(weapon["speed"])),
            int64_literal(fixed(weapon["gravity"])), weapon["lifetime_ticks"], weapon["pellet_count"],
            int64_literal(fixed(weapon["splash_radius"])), int64_literal(fixed(weapon["knockback"])),
            weapon["ammo"], weapon.get("charge_ticks", 0),
        )
        lines.append(f"        {{{values[0]}, {values[1]}u, {values[2]}, {values[3]}, {values[4]}u, {values[5]}u, {values[6]}, {values[7]}, {values[8]}u, {values[9]}u}},")
    lines.extend(["    };", "    inline constexpr SpawnRaw SpawnPoints[] =", "    {"])
    for spawn in source["level"]["spawn_points"]:
        position = ", ".join(int64_literal(fixed(component)) for component in spawn["position"])
        lines.append(f"        {{{spawn['id']}u, {{{position}}}, UINT16_C({fixed_angle(spawn['yaw'])})}},")
    lines.extend(["    };", "    inline constexpr TurretRaw Turrets[] =", "    {"])
    weapon_index = {name: index for index, name in enumerate(WEAPON_NAMES)}
    for turret in source["level"]["turrets"]:
        position = ", ".join(int64_literal(fixed(component)) for component in turret["position"])
        lines.append(f"        {{{turret['id']}u, {weapon_index[turret['archetype']]}u, {{{position}}}, UINT16_C({fixed_angle(turret['yaw'])})}},")
    pickup_type = {"health": 0, "armor": 1, "ammo": 2}
    lines.extend(["    };", "    inline constexpr PickupRaw Pickups[] =", "    {"])
    for pickup in source["level"]["pickups"]:
        position = ", ".join(int64_literal(fixed(component)) for component in pickup["position"])
        pickup_weapon = weapon_index.get(pickup.get("weapon", "nailgun"), 0)
        lines.append(f"        {{{pickup['id']}u, {pickup_type[pickup['type']]}u, {pickup_weapon}u, {{{position}}}, {pickup['amount']}}},")
    jump_pad = source["level"]["jump_pads"][0]
    pad_position = ", ".join(int64_literal(fixed(component)) for component in jump_pad["position"])
    pad_velocity = ", ".join(int64_literal(fixed(component)) for component in jump_pad["velocity"])
    lines.extend([
        "    };",
        f"    inline constexpr uint32_t JumpPadId = {jump_pad['id']}u;",
        f"    inline constexpr int64_t JumpPadPosition[] = {{{pad_position}}};",
        f"    inline constexpr int64_t JumpPadVelocity[] = {{{pad_velocity}}};",
        f"    inline constexpr uint32_t CollisionCrc = UINT32_C({collision_crc});",
        f"    inline constexpr uint32_t ConfigCrc = UINT32_C({config_crc});",
        f"    inline constexpr uint32_t LevelCrc = UINT32_C({level_crc});",
        "}",
        "",
    ])
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines), encoding="utf-8")


def write_config(path: Path, source: dict) -> int:
    payload = bytearray()
    movement_keys = (
        "ground_acceleration", "air_acceleration", "friction", "gravity", "jump_impulse",
        "standing_height", "crouched_height", "player_radius", "step_height",
        "maximum_slope_cosine", "crouched_speed_scale", "slide_minimum_speed", "slide_friction",
        "slide_acceleration_scale", "slide_maximum_speed",
    )
    for key in movement_keys:
        payload.extend(struct.pack("<q", fixed(source["movement"][key])))
    for name in WEAPON_NAMES:
        weapon = source["weapons"][name]
        encoded = name.encode("ascii")
        payload.extend(struct.pack("<B", len(encoded)) + encoded)
        for key in sorted(weapon):
            value = weapon[key]
            key_bytes = key.encode("ascii")
            payload.extend(struct.pack("<B", len(key_bytes)) + key_bytes)
            if isinstance(value, int):
                payload.extend(struct.pack("<Bq", 0, value))
            else:
                payload.extend(struct.pack("<Bq", 1, fixed(value)))
        payload.extend(b"\0")
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<4sIIII", b"A3CF", FORMAT_VERSION, 1, len(payload), checksum) + payload)
    return checksum


def write_level(path: Path, level: dict, collision_crc: int, config_crc: int) -> int:
    payload = bytearray()
    level_id = level["id"].encode("ascii")
    payload.extend(struct.pack("<B", len(level_id)) + level_id)
    payload.extend(struct.pack("<II", collision_crc, config_crc))
    payload.extend(struct.pack("<I", len(level["spawn_points"])))
    for spawn in level["spawn_points"]:
        payload.extend(struct.pack("<I3qH", spawn["id"], *(fixed(v) for v in spawn["position"]), fixed_angle(spawn["yaw"])))
    payload.extend(struct.pack("<I", len(level["turrets"])))
    for turret in level["turrets"]:
        name = turret["archetype"].encode("ascii")
        payload.extend(struct.pack("<IB", turret["id"], len(name)) + name)
        payload.extend(struct.pack("<3qH", *(fixed(v) for v in turret["position"]), fixed_angle(turret["yaw"])))
    payload.extend(struct.pack("<I", len(level["jump_pads"])))
    for pad in level["jump_pads"]:
        values = pad["position"] + pad["size"] + pad["velocity"]
        payload.extend(struct.pack("<I9q", pad["id"], *(fixed(v) for v in values)))
    payload.extend(struct.pack("<I", len(level["pickups"])))
    for pickup in level["pickups"]:
        pickup_type = pickup["type"].encode("ascii")
        weapon = pickup.get("weapon", "").encode("ascii")
        payload.extend(struct.pack("<IB", pickup["id"], len(pickup_type)) + pickup_type)
        payload.extend(struct.pack("<B", len(weapon)) + weapon)
        payload.extend(struct.pack("<3qi", *(fixed(v) for v in pickup["position"]), pickup["amount"]))
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<4sIIII", b"A3LV", LEVEL_FORMAT_VERSION, 1, len(payload), checksum) + payload)
    return checksum


def make_textures(output: Path) -> None:
    for name, colors in {
        "white": ((255, 255, 255, 255), (255, 255, 255, 255)),
        "tile": ((78, 88, 102, 255), (94, 106, 120, 255)),
        "hazard": ((242, 115, 20, 255), (34, 38, 44, 255)),
        "emissive": ((20, 255, 220, 255), (8, 96, 88, 255)),
    }.items():
        pixels = bytearray()
        for y in range(64):
            for x in range(64):
                index = ((x // 8) + (y // 8)) & 1
                pixels.extend(colors[index])
        write_png(output / "textures" / f"{name}.png", 64, 64, bytes(pixels))


def make_audio(output: Path) -> None:
    sounds = {
        "nailgun_fire": (780, 0.08), "rocket_fire": (110, 0.28), "railgun_charge": (440, 1.5),
        "railgun_fire": (1250, 0.22), "plasma_fire": (620, 0.12), "grenade_fire": (180, 0.18),
        "shotgun_fire": (90, 0.25), "impact": (260, 0.10), "pickup": (980, 0.18),
        "jump": (520, 0.12), "landing": (130, 0.12), "death": (75, 0.6), "warning": (880, 0.25),
    }
    for name, (frequency, duration) in sounds.items():
        write_tone(output / "audio" / f"{name}.wav", frequency, duration)


def build(source_path: Path, output: Path) -> None:
    source = json.loads(source_path.read_text(encoding="utf-8"))
    if source.get("format_version") != FORMAT_VERSION:
        raise ValueError("unsupported content source version")
    if output.exists():
        shutil.rmtree(output)
    output.mkdir(parents=True)

    arena, brushes, chunks = arena_mesh()
    write_glb(output / "models" / "arena01.glb", arena)
    for index, chunk in enumerate(chunks):
        write_glb(output / "models" / "arena01" / f"chunk_{index}.glb", chunk)
    for name in WEAPON_NAMES:
        write_glb(output / "models" / "weapons" / f"{name}.glb", weapon_mesh(name))
    write_glb(output / "models" / "turrets" / "base.glb", turret_mesh("base"))
    for name in WEAPON_NAMES:
        write_glb(output / "models" / "turrets" / f"{name}.glb", turret_mesh(name))
    for name in ("nail", "rocket", "plasma", "grenade", "pellet"):
        write_glb(output / "models" / "projectiles" / f"{name}.glb", projectile_mesh(name))
    for name, color in (("health", (0.9, 0.12, 0.16, 1)), ("armor", (0.12, 0.55, 0.95, 1)), ("ammo", (0.95, 0.72, 0.16, 1))):
        mesh = Mesh()
        add_box(mesh, (0, 0, 0), (0.55, 0.55, 0.55), color)
        write_glb(output / "models" / "pickups" / f"{name}.glb", mesh)

    make_textures(output)
    make_audio(output)
    config_crc = write_config(output / "data" / "arena3d.config.bin", source)
    collision_crc = write_collision(output / "data" / "arena01.collision.bin", brushes)
    level_crc = write_level(output / "data" / "arena01.level.bin", source["level"], collision_crc, config_crc)
    write_collision_header(output / "include" / "Arena3DGeneratedCollision.h", brushes, collision_crc, config_crc, level_crc, source)
    manifest = {
        "format_version": FORMAT_VERSION,
        "level_format_version": LEVEL_FORMAT_VERSION,
        "level": source["level"]["id"],
        "visual": "models/arena01.glb",
        "chunks": [f"models/arena01/chunk_{index}.glb" for index in range(len(chunks))],
        "collision": "data/arena01.collision.bin",
        "config": "data/arena3d.config.bin",
        "level_data": "data/arena01.level.bin",
        "crc": {"collision": collision_crc, "config": config_crc, "level": level_crc},
        "weapons": list(WEAPON_NAMES),
    }
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def glb_document(path: Path) -> dict:
    data = path.read_bytes()
    if len(data) < 20 or data[:4] != b"glTF" or struct.unpack_from("<I", data, 4)[0] != 2:
        raise ValueError(f"invalid GLB header: {path}")
    json_length, chunk_type = struct.unpack_from("<I4s", data, 12)
    if chunk_type != b"JSON":
        raise ValueError(f"missing JSON chunk: {path}")
    return json.loads(data[20:20 + json_length].decode("utf-8"))


def validate(source_path: Path, output: Path) -> None:
    source = json.loads(source_path.read_text(encoding="utf-8"))
    ids: list[int] = []
    for key in ("spawn_points", "turrets", "jump_pads", "pickups"):
        ids.extend(item["id"] for item in source["level"][key])
    if len(ids) != len(set(ids)):
        raise ValueError("duplicate level object ids")
    if len(source["level"]["spawn_points"]) < 4:
        raise ValueError("arena01 requires at least four spawn points")
    if set(item["archetype"] for item in source["level"]["turrets"]) - set(WEAPON_NAMES):
        raise ValueError("unknown turret archetype")
    if len(source["level"]["turrets"]) < 10:
        raise ValueError("arena01 requires ten turrets")
    valid_pickup_types = {"health", "armor", "ammo"}
    for pickup in source["level"]["pickups"]:
        if pickup["type"] not in valid_pickup_types or (pickup["type"] == "ammo" and pickup.get("weapon") not in WEAPON_NAMES):
            raise ValueError(f"invalid pickup {pickup['id']}")
    level_extent = Decimal(source["level"]["size"][0]) * Decimal("0.5")
    for spawn in source["level"]["spawn_points"]:
        position = [Decimal(component) for component in spawn["position"]]
        if abs(position[0]) >= level_extent or abs(position[2]) >= level_extent or position[1] < 0:
            raise ValueError(f"invalid spawn point {spawn['id']}")
    _, brushes, _ = arena_mesh()
    if [brush[0] for brush in brushes] != list(range(1, len(brushes) + 1)):
        raise ValueError("collision brush ids must be unique and monotonic")
    for brush_id, center, size in brushes:
        if any(component <= 0 for component in size):
            raise ValueError(f"non-positive convex brush size {brush_id}")
        if any(abs(component) > 2048 for component in center) or any(component > 4096 for component in size):
            raise ValueError(f"collision brush {brush_id} exceeds fixed-world bounds")
    player_radius = Decimal(source["movement"]["player_radius"])
    player_height = Decimal(source["movement"]["standing_height"])
    for spawn in source["level"]["spawn_points"]:
        px, py, pz = (Decimal(component) for component in spawn["position"])
        segment_minimum_y = py + player_radius
        segment_maximum_y = py + player_height - player_radius
        for brush_id, center, size in brushes:
            brush_minimum = tuple(Decimal(str(center[i])) - Decimal(str(size[i])) * Decimal("0.5") for i in range(3))
            brush_maximum = tuple(Decimal(str(center[i])) + Decimal(str(size[i])) * Decimal("0.5") for i in range(3))
            dx = max(brush_minimum[0] - px, Decimal(0), px - brush_maximum[0])
            dy = max(brush_minimum[1] - segment_maximum_y, Decimal(0), segment_minimum_y - brush_maximum[1])
            dz = max(brush_minimum[2] - pz, Decimal(0), pz - brush_maximum[2])
            overlap = dx * dx + dy * dy + dz * dz < player_radius * player_radius
            if overlap:
                raise ValueError(f"spawn point {spawn['id']} overlaps collision brush {brush_id}")
    if not (output / "include" / "Arena3DGeneratedCollision.h").is_file():
        raise ValueError("missing generated collision header")
    manifest = json.loads((output / "manifest.json").read_text(encoding="utf-8"))
    for path in sorted((output / "models").rglob("*.glb")):
        document = glb_document(path)
        primitive = document["meshes"][0]["primitives"][0]
        required = {"POSITION", "NORMAL", "COLOR_0", "TEXCOORD_0"}
        if set(primitive["attributes"]) != required or "indices" not in primitive:
            raise ValueError(f"unsupported GLB attributes: {path}")
    for key, magic in (("collision", b"A3CL"), ("config", b"A3CF"), ("level_data", b"A3LV")):
        path = output / manifest[key]
        data = path.read_bytes()
        if data[:4] != magic:
            raise ValueError(f"invalid {key} magic")
        _, version, _, payload_size, checksum = struct.unpack_from("<4sIIII", data)
        expected_version = LEVEL_FORMAT_VERSION if key == "level_data" else FORMAT_VERSION
        if version != expected_version or payload_size != len(data) - 20 or zlib.crc32(data[20:]) & 0xFFFFFFFF != checksum:
            raise ValueError(f"invalid {key} header or CRC")


def tree_digest(root: Path) -> str:
    digest = hashlib.sha256()
    for path in sorted(p for p in root.rglob("*") if p.is_file()):
        digest.update(path.relative_to(root).as_posix().encode("utf-8"))
        digest.update(b"\0")
        digest.update(path.read_bytes())
    return digest.hexdigest()


def check(source_path: Path, output: Path) -> None:
    validate(source_path, output)
    with tempfile.TemporaryDirectory(prefix="arena3d-content-") as temporary:
        rebuilt = Path(temporary) / "generated"
        build(source_path, rebuilt)
        validate(source_path, rebuilt)
        if tree_digest(output) != tree_digest(rebuilt):
            raise ValueError("generated Arena3D content is not reproducible")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("build", "validate", "check"))
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    if args.command == "build":
        build(args.source, args.output)
        validate(args.source, args.output)
    elif args.command == "validate":
        validate(args.source, args.output)
    else:
        check(args.source, args.output)
    print(f"Arena3D content {args.command}: {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
