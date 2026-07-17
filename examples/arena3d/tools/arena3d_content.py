#!/usr/bin/env python3
"""Deterministic, dependency-free Arena3D content builder.

The source JSON stores all simulation decimals as strings.  This tool converts
them directly to Q16.16 and creates the low-poly GLB, PNG, WAV and canonical
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


FORMAT_VERSION = 2
LEVEL_FORMAT_VERSION = 4
Q16_SCALE = 1 << 16
ANGLE16_TURN = 1 << 16
FIXED_TWO_PI = 411774
ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT / "content" / "arena3d_source.json"
DEFAULT_OUTPUT = ROOT / "resources" / "Data" / "generated"
WEAPON_NAMES = ("nailgun", "rocket", "railgun", "plasmagun", "grenade", "shotgun")
ARENA_VISUAL_LAYERS = ("stone", "metal", "emissive")


def fixed(value: str) -> int:
    try:
        raw = (Decimal(value) * Q16_SCALE).to_integral_value(rounding=ROUND_DOWN)
    except InvalidOperation as exc:
        raise ValueError(f"invalid fixed decimal {value!r}") from exc
    result = int(raw)
    if result < -(1 << 31) or result > (1 << 31) - 1:
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
    add_colored_quad(mesh, points, normal, (color, color, color, color), (uv_scale, uv_scale))


def add_colored_quad(mesh: Mesh, points: Sequence[Sequence[float]], normal: Sequence[float], colors: Sequence[Sequence[float]], uv_size: Sequence[float]) -> None:
    uvs = ((0.0, 0.0), (uv_size[0], 0.0), (uv_size[0], uv_size[1]), (0.0, uv_size[1]))
    add_textured_quad(mesh, points, normal, colors, uvs)


def add_textured_quad(mesh: Mesh, points: Sequence[Sequence[float]], normal: Sequence[float], colors: Sequence[Sequence[float]], uvs: Sequence[Sequence[float]]) -> None:
    base = len(mesh.positions)
    for point, color, uv in zip(points, colors, uvs):
        mesh.vertex(point, normal, color, uv)
    mesh.indices.extend((base, base + 1, base + 2, base, base + 2, base + 3))


def world_uvs(points: Sequence[Sequence[float]], normal: Sequence[float], texture_density: float) -> tuple[tuple[float, float], ...]:
    if abs(normal[1]) > 0.75:
        return tuple((point[0] * texture_density, point[2] * texture_density) for point in points)
    tangent_x = normal[2]
    tangent_z = -normal[0]
    return tuple(((point[0] * tangent_x + point[2] * tangent_z) * texture_density, point[1] * texture_density) for point in points)


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


def scale_color(color: Sequence[float], factor: float) -> tuple[float, float, float, float]:
    return tuple(max(0.0, min(1.0, color[channel] * factor)) for channel in range(3)) + (float(color[3]),)


def add_shaded_box(mesh: Mesh, center: Sequence[float], size: Sequence[float], color: Sequence[float], texture_density: float = 0.25) -> None:
    cx, cy, cz = center
    hx, hy, hz = (size[0] * 0.5, size[1] * 0.5, size[2] * 0.5)
    faces = (
        (((cx - hx, cy - hy, cz - hz), (cx - hx, cy + hy, cz - hz), (cx + hx, cy + hy, cz - hz), (cx + hx, cy - hy, cz - hz)), (0, 0, -1), (size[0], size[1]), 0.86),
        (((cx + hx, cy - hy, cz + hz), (cx + hx, cy + hy, cz + hz), (cx - hx, cy + hy, cz + hz), (cx - hx, cy - hy, cz + hz)), (0, 0, 1), (size[0], size[1]), 0.94),
        (((cx - hx, cy - hy, cz + hz), (cx - hx, cy + hy, cz + hz), (cx - hx, cy + hy, cz - hz), (cx - hx, cy - hy, cz - hz)), (-1, 0, 0), (size[2], size[1]), 0.78),
        (((cx + hx, cy - hy, cz - hz), (cx + hx, cy + hy, cz - hz), (cx + hx, cy + hy, cz + hz), (cx + hx, cy - hy, cz + hz)), (1, 0, 0), (size[2], size[1]), 0.90),
        (((cx - hx, cy + hy, cz - hz), (cx - hx, cy + hy, cz + hz), (cx + hx, cy + hy, cz + hz), (cx + hx, cy + hy, cz - hz)), (0, 1, 0), (size[0], size[2]), 1.08),
        (((cx - hx, cy - hy, cz + hz), (cx - hx, cy - hy, cz - hz), (cx + hx, cy - hy, cz - hz), (cx + hx, cy - hy, cz + hz)), (0, -1, 0), (size[0], size[2]), 0.60),
    )
    coordinate_seed = int(round((cx + 64.0) * 8.0)) * 31 + int(round((cy + 16.0) * 8.0)) * 17 + int(round((cz + 64.0) * 8.0)) * 13
    object_variation = ((coordinate_seed & 7) - 3) * 0.012
    corner_variation = (-0.035, 0.015, 0.045, -0.010)
    for points, normal, dimensions, face_factor in faces:
        colors = tuple(scale_color(color, face_factor + object_variation + variation) for variation in corner_variation)
        add_textured_quad(mesh, points, normal, colors, world_uvs(points, normal, texture_density))


def add_chamfered_box_y(mesh: Mesh, center: Sequence[float], size: Sequence[float], bevel: float, color: Sequence[float], texture_density: float = 0.25) -> None:
    cx, cy, cz = center
    hx, hy, hz = (size[0] * 0.5, size[1] * 0.5, size[2] * 0.5)
    bevel = max(0.0, min(bevel, hx * 0.45, hz * 0.45))
    ring = (
        (-hx + bevel, -hz), (hx - bevel, -hz), (hx, -hz + bevel), (hx, hz - bevel),
        (hx - bevel, hz), (-hx + bevel, hz), (-hx, hz - bevel), (-hx, -hz + bevel),
    )
    for index, current in enumerate(ring):
        following = ring[(index + 1) % len(ring)]
        dx = following[0] - current[0]
        dz = following[1] - current[1]
        length = math.sqrt(dx * dx + dz * dz)
        normal = (dz / length, 0.0, -dx / length)
        points = (
            (cx + current[0], cy - hy, cz + current[1]),
            (cx + current[0], cy + hy, cz + current[1]),
            (cx + following[0], cy + hy, cz + following[1]),
            (cx + following[0], cy - hy, cz + following[1]),
        )
        side_factor = 0.82 + 0.14 * max(0.0, normal[0]) + 0.08 * max(0.0, normal[2])
        colors = tuple(scale_color(color, side_factor + variation) for variation in (-0.025, 0.015, 0.035, -0.010))
        add_textured_quad(mesh, points, normal, colors, world_uvs(points, normal, texture_density))
    top_color = scale_color(color, 1.08)
    bottom_color = scale_color(color, 0.60)
    top_center = (cx, cy + hy, cz)
    bottom_center = (cx, cy - hy, cz)
    for index, current in enumerate(ring):
        following = ring[(index + 1) % len(ring)]
        top_current = (cx + current[0], cy + hy, cz + current[1])
        top_following = (cx + following[0], cy + hy, cz + following[1])
        bottom_current = (cx + current[0], cy - hy, cz + current[1])
        bottom_following = (cx + following[0], cy - hy, cz + following[1])
        add_triangle(mesh, (top_center, top_following, top_current), top_color)
        add_triangle(mesh, (bottom_center, bottom_current, bottom_following), bottom_color)


def add_extruded_polygon_y(mesh: Mesh, footprint: Sequence[Sequence[float]], bottom: float, top: float, color: Sequence[float], texture_density: float = 0.25) -> None:
    if len(footprint) < 3 or top <= bottom:
        return
    center_x = sum(point[0] for point in footprint) / len(footprint)
    center_z = sum(point[1] for point in footprint) / len(footprint)
    top_center = (center_x, top, center_z)
    bottom_center = (center_x, bottom, center_z)
    for index, current in enumerate(footprint):
        following = footprint[(index + 1) % len(footprint)]
        dx = following[0] - current[0]
        dz = following[1] - current[1]
        length = math.sqrt(dx * dx + dz * dz)
        if length < 1e-8:
            continue
        normal = (dz / length, 0.0, -dx / length)
        side_points = (
            (current[0], bottom, current[1]), (current[0], top, current[1]),
            (following[0], top, following[1]), (following[0], bottom, following[1]),
        )
        side_factor = 0.80 + 0.13 * max(0.0, normal[0]) + 0.08 * max(0.0, normal[2])
        side_colors = tuple(scale_color(color, side_factor + variation) for variation in (-0.02, 0.015, 0.03, -0.01))
        add_textured_quad(mesh, side_points, normal, side_colors, world_uvs(side_points, normal, texture_density))

        top_current = (current[0], top, current[1])
        top_following = (following[0], top, following[1])
        bottom_current = (current[0], bottom, current[1])
        bottom_following = (following[0], bottom, following[1])
        add_textured_triangle(mesh, (top_center, top_following, top_current), (scale_color(color, 1.08),) * 3,
            tuple((point[0] * texture_density, point[2] * texture_density) for point in (top_center, top_following, top_current)))
        add_textured_triangle(mesh, (bottom_center, bottom_current, bottom_following), (scale_color(color, 0.58),) * 3,
            tuple((point[0] * texture_density, point[2] * texture_density) for point in (bottom_center, bottom_current, bottom_following)))


def add_wedge_z(mesh: Mesh, center_x: float, width: float, start_z: float, length: float, bottom: float, low_height: float, high_height: float, color: Sequence[float], texture_density: float = 0.25) -> None:
    if width <= 0.0 or length <= 0.0 or high_height <= low_height:
        return
    x0 = center_x - width * 0.5
    x1 = center_x + width * 0.5
    z0 = start_z
    z1 = start_z + length
    a = (x0, bottom, z0)
    b = (x1, bottom, z0)
    c = (x0, bottom, z1)
    d = (x1, bottom, z1)
    al = (x0, bottom + low_height, z0)
    bl = (x1, bottom + low_height, z0)
    ch = (x0, bottom + high_height, z1)
    dh = (x1, bottom + high_height, z1)
    slope = high_height - low_height
    top_length = math.sqrt(length * length + slope * slope)
    top_normal = (0.0, length / top_length, -slope / top_length)
    top_points = (al, ch, dh, bl)
    add_textured_quad(mesh, top_points, top_normal, (scale_color(color, 1.06),) * 4, world_uvs(top_points, top_normal, texture_density))
    add_textured_triangle(mesh, (a, c, ch), (scale_color(color, 0.76),) * 3, ((0.0, 0.0), (length * texture_density, 0.0), (length * texture_density, high_height * texture_density)))
    add_textured_triangle(mesh, (a, ch, al), (scale_color(color, 0.82),) * 3, ((0.0, 0.0), (length * texture_density, high_height * texture_density), (0.0, low_height * texture_density)))
    add_textured_triangle(mesh, (b, bl, dh), (scale_color(color, 0.92),) * 3, ((0.0, 0.0), (0.0, low_height * texture_density), (length * texture_density, high_height * texture_density)))
    add_textured_triangle(mesh, (b, dh, d), (scale_color(color, 0.88),) * 3, ((0.0, 0.0), (length * texture_density, high_height * texture_density), (length * texture_density, 0.0)))
    add_colored_quad(mesh, (a, al, bl, b), (0.0, 0.0, -1.0), (scale_color(color, 0.72),) * 4, (width * texture_density, low_height * texture_density))
    add_colored_quad(mesh, (c, d, dh, ch), (0.0, 0.0, 1.0), (scale_color(color, 0.92),) * 4, (width * texture_density, high_height * texture_density))


def add_arch_z(mesh: Mesh, center_x: float, spring_y: float, center_z: float, inner_radius: float, thickness: float, depth: float, segments: int, color: Sequence[float], texture_density: float = 0.25) -> None:
    if inner_radius <= 0.0 or thickness <= 0.0 or depth <= 0.0 or segments < 3:
        return
    outer_radius = inner_radius + thickness
    front_z = center_z - depth * 0.5
    back_z = center_z + depth * 0.5
    for segment in range(segments):
        angle0 = math.pi * segment / segments
        angle1 = math.pi * (segment + 1) / segments
        cos0, sin0 = math.cos(angle0), math.sin(angle0)
        cos1, sin1 = math.cos(angle1), math.sin(angle1)
        inner0_front = (center_x + cos0 * inner_radius, spring_y + sin0 * inner_radius, front_z)
        inner1_front = (center_x + cos1 * inner_radius, spring_y + sin1 * inner_radius, front_z)
        outer0_front = (center_x + cos0 * outer_radius, spring_y + sin0 * outer_radius, front_z)
        outer1_front = (center_x + cos1 * outer_radius, spring_y + sin1 * outer_radius, front_z)
        inner0_back = (inner0_front[0], inner0_front[1], back_z)
        inner1_back = (inner1_front[0], inner1_front[1], back_z)
        outer0_back = (outer0_front[0], outer0_front[1], back_z)
        outer1_back = (outer1_front[0], outer1_front[1], back_z)
        face_uvs = ((angle0 * outer_radius * texture_density, 0.0), (angle0 * inner_radius * texture_density, thickness * texture_density),
            (angle1 * inner_radius * texture_density, thickness * texture_density), (angle1 * outer_radius * texture_density, 0.0))
        add_textured_quad(mesh, (outer0_front, inner0_front, inner1_front, outer1_front), (0.0, 0.0, -1.0), (scale_color(color, 0.94),) * 4, face_uvs)
        add_textured_quad(mesh, (outer0_back, outer1_back, inner1_back, inner0_back), (0.0, 0.0, 1.0), (scale_color(color, 1.02),) * 4, face_uvs)
        middle_angle = (angle0 + angle1) * 0.5
        outer_normal = (math.cos(middle_angle), math.sin(middle_angle), 0.0)
        inner_normal = (-outer_normal[0], -outer_normal[1], 0.0)
        add_colored_quad(mesh, (outer0_front, outer1_front, outer1_back, outer0_back), outer_normal, (scale_color(color, 0.88),) * 4, ((angle1 - angle0) * outer_radius * texture_density, depth * texture_density))
        add_colored_quad(mesh, (inner0_back, inner1_back, inner1_front, inner0_front), inner_normal, (scale_color(color, 0.72),) * 4, ((angle1 - angle0) * inner_radius * texture_density, depth * texture_density))


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


def add_cylinder_z(mesh: Mesh, center: Sequence[float], radius: float, length: float, sides: int, color: Sequence[float]) -> None:
    cx, cy, cz = center
    z0, z1 = cz - length * 0.5, cz + length * 0.5
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        p0 = (cx + math.cos(a0) * radius, cy + math.sin(a0) * radius, z0)
        p1 = (cx + math.cos(a1) * radius, cy + math.sin(a1) * radius, z0)
        p2 = (p1[0], p1[1], z1)
        p3 = (p0[0], p0[1], z1)
        normal = (math.cos((a0 + a1) * 0.5), math.sin((a0 + a1) * 0.5), 0.0)
        add_quad(mesh, (p0, p1, p2, p3), normal, color)
    front = mesh.vertex((cx, cy, z1), (0, 0, 1), color, (0.5, 0.5))
    back = mesh.vertex((cx, cy, z0), (0, 0, -1), color, (0.5, 0.5))
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        f0 = mesh.vertex((cx + math.cos(a0) * radius, cy + math.sin(a0) * radius, z1), (0, 0, 1), color, (0, 0))
        f1 = mesh.vertex((cx + math.cos(a1) * radius, cy + math.sin(a1) * radius, z1), (0, 0, 1), color, (1, 0))
        b0 = mesh.vertex((cx + math.cos(a0) * radius, cy + math.sin(a0) * radius, z0), (0, 0, -1), color, (0, 0))
        b1 = mesh.vertex((cx + math.cos(a1) * radius, cy + math.sin(a1) * radius, z0), (0, 0, -1), color, (1, 0))
        mesh.indices.extend((front, f0, f1, back, b1, b0))


def add_cylinder_x(mesh: Mesh, center: Sequence[float], radius: float, length: float, sides: int, color: Sequence[float]) -> None:
    cx, cy, cz = center
    x0, x1 = cx - length * 0.5, cx + length * 0.5
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        p0 = (x0, cy + math.cos(a0) * radius, cz + math.sin(a0) * radius)
        p1 = (x0, cy + math.cos(a1) * radius, cz + math.sin(a1) * radius)
        p2 = (x1, p1[1], p1[2])
        p3 = (x1, p0[1], p0[2])
        normal = (0.0, math.cos((a0 + a1) * 0.5), math.sin((a0 + a1) * 0.5))
        add_quad(mesh, (p0, p1, p2, p3), normal, color)
    front = mesh.vertex((x1, cy, cz), (1, 0, 0), color, (0.5, 0.5))
    back = mesh.vertex((x0, cy, cz), (-1, 0, 0), color, (0.5, 0.5))
    for side in range(sides):
        a0 = 2.0 * math.pi * side / sides
        a1 = 2.0 * math.pi * (side + 1) / sides
        f0 = mesh.vertex((x1, cy + math.cos(a0) * radius, cz + math.sin(a0) * radius), (1, 0, 0), color, (0, 0))
        f1 = mesh.vertex((x1, cy + math.cos(a1) * radius, cz + math.sin(a1) * radius), (1, 0, 0), color, (1, 0))
        b0 = mesh.vertex((x0, cy + math.cos(a0) * radius, cz + math.sin(a0) * radius), (-1, 0, 0), color, (0, 0))
        b1 = mesh.vertex((x0, cy + math.cos(a1) * radius, cz + math.sin(a1) * radius), (-1, 0, 0), color, (1, 0))
        mesh.indices.extend((front, f0, f1, back, b1, b0))


def add_colored_triangle(mesh: Mesh, points: Sequence[Sequence[float]], colors: Sequence[Sequence[float]]) -> None:
    add_textured_triangle(mesh, points, colors, ((0.5, 1.0), (0.0, 0.0), (1.0, 0.0)))


def add_textured_triangle(mesh: Mesh, points: Sequence[Sequence[float]], colors: Sequence[Sequence[float]], uvs: Sequence[Sequence[float]]) -> None:
    ux, uy, uz = (points[1][axis] - points[0][axis] for axis in range(3))
    vx, vy, vz = (points[2][axis] - points[0][axis] for axis in range(3))
    nx = uy * vz - uz * vy
    ny = uz * vx - ux * vz
    nz = ux * vy - uy * vx
    length = math.sqrt(nx * nx + ny * ny + nz * nz)
    if length < 1e-8:
        return
    normal = (nx / length, ny / length, nz / length)
    base = len(mesh.positions)
    for point, color, uv in zip(points, colors, uvs):
        mesh.vertex(point, normal, color, uv)
    mesh.indices.extend((base, base + 1, base + 2))


def add_triangle(mesh: Mesh, points: Sequence[Sequence[float]], color: Sequence[float]) -> None:
    add_colored_triangle(mesh, points, (color, color, color))


def add_octahedron(mesh: Mesh, center: Sequence[float], radius: float, color: Sequence[float]) -> None:
    cx, cy, cz = center
    top = (cx, cy + radius, cz)
    bottom = (cx, cy - radius, cz)
    east = (cx + radius, cy, cz)
    north = (cx, cy, cz + radius)
    west = (cx - radius, cy, cz)
    south = (cx, cy, cz - radius)
    for face in (
        (top, north, east), (top, west, north), (top, south, west), (top, east, south),
        (bottom, east, north), (bottom, north, west), (bottom, west, south), (bottom, south, east),
    ):
        add_triangle(mesh, face, color)


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


def arena_collision() -> list[tuple[int, tuple[float, float, float], tuple[float, float, float]]]:
    brushes: list[tuple[int, tuple[float, float, float], tuple[float, float, float]]] = []

    def solid(center: tuple[float, float, float], size: tuple[float, float, float]) -> None:
        brushes.append((len(brushes) + 1, center, size))

    def stairs_z(x: float, start_z: float, count: int, direction: float = 1.0) -> None:
        depth = 0.62
        rise = 0.25
        for index in range(count):
            top = rise * (index + 1)
            z = start_z + direction * depth * index
            solid((x, top * 0.5, z), (2.8, top, depth + 0.04))

    # Collision is authored independently from presentation. Keep it simple,
    # stable and conservative even when the visible architecture is rebuilt.
    solid((0, -0.5, -20), (63, 1, 24))
    solid((-22, -0.5, 8), (20, 1, 32))
    solid((0, -0.5, 0), (24, 1, 16))
    solid((22, -0.5, -2), (20, 1, 12))
    solid((22, -0.5, 18), (20, 1, 12))
    solid((0, -0.5, 28), (63, 1, 8))
    solid((22, -3.0, 8), (20, 1, 8))

    for x in (-16, 16):
        solid((x, 6, -32), (32, 12, 1))
        solid((x, 6, 32), (32, 12, 1))
    for z in (-16, 16):
        solid((-32, 6, z), (1, 12, 32))
        solid((32, 6, z), (1, 12, 32))
    solid((-12, 2.0, 16), (1, 4, 16))
    solid((12, 2.0, 8), (1, 4, 16))
    solid((12, 2.0, 20), (1, 4, 8))
    solid((0, 1.5, -8), (13, 3, 1))

    stairs_z(-7.2, -5.8, 16)
    stairs_z(7.2, -5.8, 16)
    solid((0, 3.75, 14), (10, 0.5, 20))
    solid((-5.5, 2.0, 14), (1, 4, 20))
    solid((5.5, 2.0, 14), (1, 4, 20))

    solid((-24, 5.75, 13), (14, 0.5, 18))
    solid((-30.5, 3.0, 13), (1, 6, 18))
    solid((22, 4.75, 18), (18, 0.5, 12))
    solid((0, 4.75, -11), (24, 0.5, 4))
    solid((-18, 4.75, -11), (12, 0.5, 4))
    solid((18, 4.75, -11), (12, 0.5, 4))

    for x, z in ((-10, -10), (10, -10), (-9, -17), (9, -17), (-20, -2), (20, -2), (-20, 24), (20, 24)):
        cover_size = 4.0 if (x, z) in ((-10, -10), (10, -10)) else 3.5
        solid((x, 1.25, z), (cover_size, 2.5, cover_size))
    for x in (-4, 4):
        solid((x, 2.0, 12), (1.2, 4, 1.2))
    for x in (-28, -20):
        solid((x, 3.0, 7), (1, 6, 1))
    for x in (16, 28):
        solid((x, 2.5, 17), (1, 5, 1))

    return brushes


def arena_mesh(level: dict) -> tuple[Mesh, dict[str, list[Mesh]]]:
    mesh = Mesh()
    layers = {name: [Mesh() for _ in range(4)] for name in ARENA_VISUAL_LAYERS}
    floor = (0.66, 0.69, 0.74, 1.0)
    wall = (0.74, 0.76, 0.80, 1.0)
    dark_stone = (0.34, 0.40, 0.48, 1.0)
    blue_stone = (0.42, 0.52, 0.62, 1.0)
    cover = (0.28, 0.58, 0.62, 1.0)
    stair = (0.68, 0.44, 0.19, 1.0)
    metal = (0.62, 0.69, 0.76, 1.0)
    dark_metal = (0.27, 0.34, 0.42, 1.0)
    glow = (0.15, 1.0, 0.82, 1.0)

    def chunk_index(center: Sequence[float]) -> int:
        return (1 if center[0] >= 0 else 0) + (2 if center[2] >= 0 else 0)

    def visual_box(layer: str, center: tuple[float, float, float], size: tuple[float, float, float], color: tuple[float, float, float, float], texture_density: float = 0.25) -> None:
        add_shaded_box(mesh, center, size, color, texture_density)
        add_shaded_box(layers[layer][chunk_index(center)], center, size, color, texture_density)

    def visual_chamfered_box(layer: str, center: tuple[float, float, float], size: tuple[float, float, float], bevel: float, color: tuple[float, float, float, float]) -> None:
        add_chamfered_box_y(mesh, center, size, bevel, color)
        add_chamfered_box_y(layers[layer][chunk_index(center)], center, size, bevel, color)

    def visual_cylinder(layer: str, center: tuple[float, float, float], radius: float, height: float, sides: int, color: tuple[float, float, float, float]) -> None:
        add_cylinder(mesh, center, radius, height, sides, color)
        add_cylinder(layers[layer][chunk_index(center)], center, radius, height, sides, color)

    def visual_cylinder_z(layer: str, center: tuple[float, float, float], radius: float, length: float, sides: int, color: tuple[float, float, float, float]) -> None:
        add_cylinder_z(mesh, center, radius, length, sides, color)
        add_cylinder_z(layers[layer][chunk_index(center)], center, radius, length, sides, color)

    def visual_prism(layer: str, footprint: Sequence[Sequence[float]], bottom: float, top: float, color: tuple[float, float, float, float], texture_density: float = 0.25) -> None:
        center = (sum(point[0] for point in footprint) / len(footprint), (bottom + top) * 0.5, sum(point[1] for point in footprint) / len(footprint))
        add_extruded_polygon_y(mesh, footprint, bottom, top, color, texture_density)
        add_extruded_polygon_y(layers[layer][chunk_index(center)], footprint, bottom, top, color, texture_density)

    def visual_wedge_z(layer: str, center_x: float, width: float, start_z: float, length: float, bottom: float, low_height: float, high_height: float, color: tuple[float, float, float, float]) -> None:
        center = (center_x, bottom + high_height * 0.5, start_z + length * 0.5)
        add_wedge_z(mesh, center_x, width, start_z, length, bottom, low_height, high_height, color, 0.4)
        add_wedge_z(layers[layer][chunk_index(center)], center_x, width, start_z, length, bottom, low_height, high_height, color, 0.4)

    def visual_arch_z(layer: str, center_x: float, spring_y: float, center_z: float, inner_radius: float, thickness: float, depth: float, segments: int, color: tuple[float, float, float, float]) -> None:
        center = (center_x, spring_y + inner_radius, center_z)
        add_arch_z(mesh, center_x, spring_y, center_z, inner_radius, thickness, depth, segments, color, 0.5)
        add_arch_z(layers[layer][chunk_index(center)], center_x, spring_y, center_z, inner_radius, thickness, depth, segments, color, 0.5)

    def regular_polygon(center_x: float, center_z: float, radius: float, sides: int, rotation: float = math.pi / 8.0) -> tuple[tuple[float, float], ...]:
        return tuple((center_x + math.cos(rotation + side * math.tau / sides) * radius,
            center_z + math.sin(rotation + side * math.tau / sides) * radius) for side in range(sides))

    def oriented_rectangle(center_x: float, center_z: float, half_length: float, half_width: float, angle: float) -> tuple[tuple[float, float], ...]:
        forward = (math.cos(angle), math.sin(angle))
        side = (-forward[1], forward[0])
        return tuple((center_x + forward[0] * along * half_length + side[0] * across * half_width,
            center_z + forward[1] * along * half_length + side[1] * across * half_width)
            for along, across in ((-1.0, -1.0), (1.0, -1.0), (1.0, 1.0), (-1.0, 1.0)))

    def stairs_z(x: float, start_z: float, count: int, direction: float = 1.0) -> None:
        depth = 0.62
        rise = 0.25
        for index in range(count):
            top = rise * (index + 1)
            z = start_z + direction * depth * index
            visual_box("stone", (x, top * 0.5, z), (2.8, top, depth + 0.04), stair, 0.55)
            visual_box("metal", (x, top - 0.045, z + direction * depth * 0.47), (2.86, 0.09, 0.11), metal, 1.4)
        if direction > 0.0:
            start_edge = start_z - depth * 0.52
            total_length = depth * count + 0.04
            for side_x in (x - 1.52, x + 1.52):
                visual_wedge_z("metal", side_x, 0.22, start_edge, total_length, 0.0, 0.18, rise * count + 0.18, dark_metal)

    # The presentation shell deliberately does not emit collision. UV density
    # is based on world dimensions, so long walls tile instead of stretching a
    # single square across the entire face.
    visual_box("stone", (0, -0.5, -20), (63, 1, 24), floor)
    visual_box("stone", (-22, -0.5, 8), (20, 1, 32), floor)
    visual_box("stone", (0, -0.5, 0), (24, 1, 16), floor)
    visual_box("stone", (22, -0.5, -2), (20, 1, 12), floor)
    visual_box("stone", (22, -0.5, 18), (20, 1, 12), floor)
    visual_box("stone", (0, -0.5, 28), (63, 1, 8), floor)
    visual_box("metal", (22, -3.0, 8), (20, 1, 8), dark_metal)

    for x in (-16, 16):
        visual_box("stone", (x, 6, -32), (32, 12, 1), wall)
        visual_box("stone", (x, 6, 32), (32, 12, 1), wall)
    for z in (-16, 16):
        visual_box("stone", (-32, 6, z), (1, 12, 32), wall)
        visual_box("stone", (32, 6, z), (1, 12, 32), wall)
    visual_box("stone", (-12, 2.0, 16), (1, 4, 16), wall)
    visual_box("stone", (12, 2.0, 8), (1, 4, 16), wall)
    visual_box("stone", (12, 2.0, 20), (1, 4, 8), wall)
    visual_box("metal", (0, 1.5, -8), (13, 3, 1), dark_metal)

    stairs_z(-7.2, -5.8, 16)
    stairs_z(7.2, -5.8, 16)
    visual_box("stone", (0, 3.75, 14), (10, 0.5, 20), wall)
    visual_box("metal", (-5.5, 2.0, 14), (1, 4, 20), dark_metal)
    visual_box("metal", (5.5, 2.0, 14), (1, 4, 20), dark_metal)

    visual_box("stone", (-24, 5.75, 13), (14, 0.5, 18), wall)
    visual_box("metal", (-30.5, 3.0, 13), (1, 6, 18), dark_metal)
    visual_box("stone", (22, 4.75, 18), (18, 0.5, 12), wall)
    visual_box("stone", (0, 4.75, -11), (24, 0.5, 4), wall)
    visual_box("stone", (-18, 4.75, -11), (12, 0.5, 4), wall)
    visual_box("stone", (18, 4.75, -11), (12, 0.5, 4), wall)

    # Central courtyard art pass. Thin nested octagons and radial emissive
    # markers sit on the existing floor brush, so the authoritative surface
    # and movement remain unchanged while the room gains a readable landmark.
    visual_prism("stone", regular_polygon(0.0, -18.0, 6.4, 8), 0.004, 0.026, dark_stone, 0.55)
    visual_prism("metal", regular_polygon(0.0, -18.0, 5.72, 8), 0.026, 0.046, dark_metal, 0.8)
    visual_prism("stone", regular_polygon(0.0, -18.0, 5.12, 8), 0.046, 0.064, blue_stone, 0.65)
    for marker in range(8):
        angle = marker * math.tau / 8.0
        marker_x = math.cos(angle) * 4.64
        marker_z = -18.0 + math.sin(angle) * 4.64
        visual_prism("emissive", oriented_rectangle(marker_x, marker_z, 0.58, 0.075, angle), 0.064, 0.084, glow, 1.0)

    # The upper route is framed as a stone portal rather than a rectangular
    # tunnel mouth. Its pillars cover the existing side-wall brushes and the
    # repeated metal ribs live entirely above the playable corridor.
    visual_chamfered_box("stone", (-5.10, 2.0, 4.15), (0.80, 4.0, 1.18), 0.16, blue_stone)
    visual_chamfered_box("stone", (5.10, 2.0, 4.15), (0.80, 4.0, 1.18), 0.16, blue_stone)
    for pillar_x in (-5.10, 5.10):
        visual_chamfered_box("metal", (pillar_x, 0.18, 4.15), (1.10, 0.36, 1.44), 0.18, dark_metal)
        visual_chamfered_box("metal", (pillar_x, 3.86, 4.15), (1.12, 0.28, 1.42), 0.18, metal)
    visual_arch_z("stone", 0.0, 4.0, 4.15, 4.70, 0.80, 0.68, 14, blue_stone)
    for rib_z in (8.0, 13.2, 18.4, 23.2):
        visual_arch_z("metal", 0.0, 4.0, rib_z, 4.82, 0.18, 0.16, 14, metal)
        visual_box("emissive", (0.0, 9.00, rib_z - 0.10), (1.30, 0.10, 0.06), glow, 1.2)

    # Structural layers below the upper platform keep the broad collision slab
    # but visually turn it into a supported bridge.
    for beam_z in (5.2, 9.4, 13.6, 17.8, 22.0):
        visual_chamfered_box("metal", (0.0, 3.44, beam_z), (10.5, 0.22, 0.34), 0.08, dark_metal)
    for pillar_x in (-4.0, 4.0):
        visual_chamfered_box("stone", (pillar_x, 0.16, 12.0), (1.55, 0.32, 1.55), 0.28, dark_stone)
        visual_chamfered_box("stone", (pillar_x, 3.82, 12.0), (1.48, 0.28, 1.48), 0.24, blue_stone)

    # The solid central barrier becomes a reactor facade. Every protruding part
    # remains inside or directly against its existing collision box.
    for tower_x in (-5.72, 5.72):
        visual_chamfered_box("metal", (tower_x, 1.50, -8.0), (1.25, 3.35, 1.34), 0.22, dark_metal)
        visual_box("emissive", (tower_x, 1.52, -8.69), (0.22, 1.65, 0.05), glow, 1.0)
    visual_cylinder_z("metal", (0.0, 1.50, -8.59), 1.62, 0.18, 16, dark_metal)
    visual_cylinder_z("metal", (0.0, 1.50, -8.70), 1.28, 0.08, 16, metal)
    visual_cylinder_z("emissive", (0.0, 1.50, -8.76), 0.92, 0.055, 16, glow)
    for reactor_x in (-3.55, 3.55):
        visual_box("metal", (reactor_x, 1.5, -8.69), (1.65, 0.20, 0.08), metal, 1.2)
        visual_box("emissive", (reactor_x, 1.5, -8.75), (0.82, 0.07, 0.035), glow, 1.2)

    # Covers use chamfered presentation meshes while collision stays a cheap
    # conservative box. Pillars are twelve-sided visually but retain AABBs in
    # the deterministic world.
    for x, z in ((-10, -10), (10, -10), (-9, -17), (9, -17), (-20, -2), (20, -2), (-20, 24), (20, 24)):
        cover_size = 4.0 if (x, z) in ((-10, -10), (10, -10)) else 3.5
        visual_chamfered_box("stone", (x, 1.25, z), (cover_size, 2.5, cover_size), 0.42, cover)
        visual_box("metal", (x, 2.38, z), (cover_size * 0.72, 0.14, cover_size * 0.72), metal, 1.25)
    for x in (-4, 4):
        visual_cylinder("metal", (x, 2.0, 12), 0.6, 4, 12, dark_metal)
    for x in (-28, -20):
        visual_cylinder("metal", (x, 3.0, 7), 0.5, 6, 12, dark_metal)
    for x in (16, 28):
        visual_cylinder("metal", (x, 2.5, 17), 0.5, 5, 12, dark_metal)

    # Non-colliding architectural detail: wall ribs, platform edge beams and
    # luminous navigation panels. These add silhouette and surface variation
    # without changing movement, sweeps or replay CRCs.
    for z, inner_z in ((-32, -31.45), (32, 31.45)):
        for x in (-16, 16):
            for y in (2.25, 7.75):
                visual_box("metal", (x, y, inner_z), (30.5, 0.20, 0.10), metal, 1.5)
            visual_chamfered_box("stone", (x, 9.55, inner_z), (31.0, 0.62, 0.48), 0.18, blue_stone)
            for offset in (-12, -6, 0, 6, 12):
                visual_chamfered_box("stone", (x + offset, 5.0, inner_z), (0.64, 8.65, 0.46), 0.15, dark_stone)
                visual_box("metal", (x + offset, 5.0, inner_z - (0.06 if inner_z > 0.0 else -0.06)), (0.16, 6.25, 0.10), metal, 1.5)
            for offset in (-9, -3, 3, 9):
                visual_chamfered_box("metal", (x + offset, 5.0, inner_z), (4.65, 3.65, 0.16), 0.18, dark_metal)
    for x, inner_x in ((-32, -31.45), (32, 31.45)):
        for z in (-16, 16):
            for y in (2.25, 7.75):
                visual_box("metal", (inner_x, y, z), (0.10, 0.20, 30.5), metal, 1.5)
            visual_chamfered_box("stone", (inner_x, 9.55, z), (0.48, 0.62, 31.0), 0.18, blue_stone)
            for offset in (-12, -6, 0, 6, 12):
                visual_chamfered_box("stone", (inner_x, 5.0, z + offset), (0.46, 8.65, 0.64), 0.15, dark_stone)
                visual_box("metal", (inner_x - (0.06 if inner_x > 0.0 else -0.06), 5.0, z + offset), (0.10, 6.25, 0.16), metal, 1.5)
            for offset in (-9, -3, 3, 9):
                visual_chamfered_box("metal", (inner_x, 5.0, z + offset), (0.16, 3.65, 4.65), 0.18, dark_metal)

    for center, size in (
        ((-4.92, 3.78, 14), (0.16, 0.20, 19.4)), ((4.92, 3.78, 14), (0.16, 0.20, 19.4)),
        ((-24, 5.79, 4.08), (13.4, 0.18, 0.16)), ((-24, 5.79, 21.92), (13.4, 0.18, 0.16)),
        ((22, 4.79, 12.08), (17.4, 0.18, 0.16)), ((22, 4.79, 23.92), (17.4, 0.18, 0.16)),
    ):
        visual_box("metal", center, size, metal, 1.5)

    for x, z in ((-20, -20), (20, -20), (-20, 24), (20, 24)):
        visual_box("emissive", (x, 0.025, z), (5.0, 0.05, 0.16), glow, 1.0)
    for x, z in ((-16, -31.38), (16, -31.38), (-16, 31.38), (16, 31.38)):
        visual_box("emissive", (x, 4.5, z), (3.6, 0.55, 0.08), glow, 1.0)

    for pad in level["jump_pads"]:
        position = tuple(float(component) for component in pad["position"])
        size = tuple(float(component) for component in pad["size"])
        visual_chamfered_box("emissive", (position[0], position[1] + 0.04, position[2]), (size[0], 0.08, size[2]), 0.22, glow)
    return mesh, layers


def lava_mesh(level: dict) -> Mesh:
    mesh = Mesh()
    for hazard in level.get("hazards", []):
        if hazard["type"] != "lava":
            continue
        center = tuple(float(component) for component in hazard["position"])
        size = tuple(float(component) for component in hazard["size"])
        surface_y = center[1] + size[1] * 0.5 - 0.02
        add_box(mesh, (center[0], surface_y, center[2]), (size[0], 0.08, size[2]), (1.0, 0.34, 0.025, 1.0), max(size[0], size[2]) / 3.0)
    return mesh


def sky_mesh() -> Mesh:
    mesh = Mesh()
    radius = 140.0
    segments = 48
    bands = 24

    def sky_color(height: float) -> tuple[float, float, float, float]:
        normalized = max(-1.0, min(1.0, height / radius))
        if normalized >= 0.0:
            t = normalized
            horizon = (0.20, 0.31, 0.52)
            zenith = (0.025, 0.055, 0.14)
            return tuple(horizon[channel] + (zenith[channel] - horizon[channel]) * t for channel in range(3)) + (1.0,)
        t = -normalized
        horizon = (0.12, 0.17, 0.28)
        nadir = (0.018, 0.025, 0.055)
        return tuple(horizon[channel] + (nadir[channel] - horizon[channel]) * t for channel in range(3)) + (1.0,)

    def point(latitude: float, longitude: float) -> tuple[float, float, float]:
        horizontal = math.cos(latitude) * radius
        return (math.sin(longitude) * horizontal, math.sin(latitude) * radius, math.cos(longitude) * horizontal)

    for band in range(bands):
        latitude0 = -math.pi * 0.5 + math.pi * band / bands
        latitude1 = -math.pi * 0.5 + math.pi * (band + 1) / bands
        for segment in range(segments):
            longitude0 = 2.0 * math.pi * segment / segments
            longitude1 = 2.0 * math.pi * (segment + 1) / segments
            p00 = point(latitude0, longitude0)
            p01 = point(latitude0, longitude1)
            p10 = point(latitude1, longitude0)
            p11 = point(latitude1, longitude1)
            c00, c01 = sky_color(p00[1]), sky_color(p01[1])
            c10, c11 = sky_color(p10[1]), sky_color(p11[1])
            # Reversed winding makes the sphere visible from the inside.
            add_colored_triangle(mesh, (p00, p11, p01), (c00, c11, c01))
            add_colored_triangle(mesh, (p00, p10, p11), (c00, c10, c11))
    return mesh


def weapon_mesh(name: str) -> Mesh:
    palette = {
        "nailgun": (0.45, 0.75, 0.95, 1), "rocket": (0.95, 0.35, 0.12, 1),
        "railgun": (0.65, 0.25, 0.95, 1), "plasmagun": (0.10, 0.95, 0.70, 1),
        "grenade": (0.55, 0.75, 0.20, 1), "shotgun": (0.95, 0.70, 0.20, 1),
    }
    mesh = Mesh()
    color = palette[name]
    add_box(mesh, (0, 0, 0), (0.55, 0.38, 1.4), color)
    add_box(mesh, (0, -0.28, -0.2), (0.3, 0.65, 0.35), (0.16, 0.18, 0.22, 1))
    if name in ("rocket", "railgun", "plasmagun"):
        add_cylinder_z(mesh, (0, 0.08, 0.9), 0.18 if name != "railgun" else 0.11, 0.9, 8, color)
    elif name == "shotgun":
        for x in (-0.13, 0.13):
            add_cylinder_z(mesh, (x, 0.08, 0.9), 0.09, 0.9, 8, color)
    else:
        add_box(mesh, (0, 0.06, 0.8), (0.18, 0.18, 0.8), color)
    return mesh


def player_mesh() -> Mesh:
    mesh = Mesh()
    armor = (0.16, 0.68, 0.82, 1.0)
    armor_dark = (0.08, 0.24, 0.34, 1.0)
    accent = (1.0, 0.42, 0.12, 1.0)
    add_chamfered_box_y(mesh, (0.0, 1.08, 0.0), (0.72, 0.72, 0.42), 0.10, armor)
    add_chamfered_box_y(mesh, (0.0, 1.62, 0.0), (0.46, 0.42, 0.40), 0.08, accent)
    add_box(mesh, (-0.46, 1.06, 0.0), (0.18, 0.70, 0.22), armor_dark)
    add_box(mesh, (0.46, 1.06, 0.0), (0.18, 0.70, 0.22), armor_dark)
    add_box(mesh, (-0.22, 0.42, 0.0), (0.24, 0.72, 0.28), armor_dark)
    add_box(mesh, (0.22, 0.42, 0.0), (0.24, 0.72, 0.28), armor_dark)
    add_box(mesh, (0.0, 1.08, 0.27), (0.34, 0.14, 0.16), accent)
    return mesh


def turret_mesh(module: str) -> Mesh:
    mesh = Mesh()
    if module == "base":
        add_cylinder(mesh, (0, 0.35, 0), 0.75, 0.7, 8, (0.22, 0.25, 0.30, 1))
        add_box(mesh, (0, 0.8, 0), (0.8, 0.35, 0.8), (0.35, 0.40, 0.48, 1))
        return mesh
    colors = {name: weapon_mesh(name).colors[0] for name in WEAPON_NAMES}
    add_box(mesh, (0, 0, 0), (0.65, 0.45, 0.65), colors[module])
    add_cylinder_z(mesh, (0, 0, 0.65), 0.12, 0.9, 8, colors[module])
    return mesh


def projectile_mesh(name: str) -> Mesh:
    mesh = Mesh()
    colors = {
        "nail": (0.7, 0.85, 1, 1), "rocket": (1, 0.35, 0.08, 1),
        "plasma": (0.05, 1, 0.8, 1), "grenade": (0.45, 0.65, 0.18, 1),
        "pellet": (1, 0.85, 0.35, 1),
    }
    if name in ("plasma", "grenade"):
        add_cylinder_x(mesh, (0, 0, 0), 0.12, 0.24, 8, colors[name])
    else:
        add_box(mesh, (0, 0, 0), (0.45 if name != "rocket" else 0.7, 0.08, 0.08), colors[name])
    return mesh


def explosion_cloud_mesh(name: str) -> Mesh:
    palettes = {
        "rocket": ((1.0, 0.30, 0.04, 0.36), (1.0, 0.72, 0.12, 0.28), (0.42, 0.10, 0.03, 0.22)),
        "plasma": ((0.02, 1.0, 0.72, 0.34), (0.10, 0.72, 1.0, 0.26), (0.02, 0.28, 0.34, 0.20)),
        "grenade": ((1.0, 0.62, 0.08, 0.32), (0.52, 0.46, 0.28, 0.25), (0.16, 0.18, 0.16, 0.20)),
    }
    mesh = Mesh()
    colors = palettes[name]
    add_octahedron(mesh, (0.0, 0.0, 0.0), 0.52, colors[0])
    add_octahedron(mesh, (0.30, 0.16, 0.10), 0.36, colors[1])
    add_octahedron(mesh, (-0.24, 0.10, -0.18), 0.31, colors[2])
    return mesh


def impact_flash_mesh() -> Mesh:
    mesh = Mesh()
    add_octahedron(mesh, (0.0, 0.0, 0.0), 0.52, (1.0, 1.0, 1.0, 0.88))
    add_octahedron(mesh, (0.12, 0.04, -0.08), 0.30, (1.0, 1.0, 1.0, 0.52))
    for axis in range(3):
        for sign in (-1.0, 1.0):
            tip = [0.0, 0.0, 0.0]
            side0 = [0.0, 0.0, 0.0]
            side1 = [0.0, 0.0, 0.0]
            tip[axis] = sign
            side0[(axis + 1) % 3] = 0.10
            side1[(axis + 2) % 3] = 0.10
            add_triangle(mesh, (side0, side1, tip), (1.0, 1.0, 1.0, 0.62))
    return mesh


def decal_mesh() -> Mesh:
    mesh = Mesh()
    add_quad(mesh,
        ((0.5, -0.5, 0.0), (0.5, 0.5, 0.0), (-0.5, 0.5, 0.0), (-0.5, -0.5, 0.0)),
        (0.0, 0.0, 1.0), (1.0, 1.0, 1.0, 1.0))
    return mesh


def box_planes(center: Sequence[float], size: Sequence[float]) -> list[tuple[int, int, int, int]]:
    cx, cy, cz = (fixed(str(v)) for v in center)
    hx, hy, hz = (fixed(str(v / 2.0)) for v in size)
    one = Q16_SCALE
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
        payload.extend(struct.pack("<III6i", brush_id, 1, len(planes), *(minimum + maximum)))
        for plane in planes:
            payload.extend(struct.pack("<4i", *plane))

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
    payload.extend(struct.pack("<iI", fixed(str(cell_size)), len(cells)))
    for (cell_x, cell_z), brush_ids in sorted(cells.items()):
        payload.extend(struct.pack("<iiI", cell_x, cell_z, len(brush_ids)))
        payload.extend(struct.pack(f"<{len(brush_ids)}I", *sorted(brush_ids)))

    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<4sIIII", b"A3CL", FORMAT_VERSION, len(brushes), len(payload), checksum) + payload)
    return checksum


def int32_literal(value: int) -> str:
    return f"-INT32_C({-value})" if value < 0 else f"INT32_C({value})"


def write_collision_header(path: Path, brushes: Sequence[tuple[int, Sequence[float], Sequence[float]]],
                           collision_crc: int, config_crc: int, level_crc: int, source: dict) -> None:
    lines = [
        "#pragma once",
        "",
        "#include <cstddef>",
        "#include <cstdint>",
        "#include \"kinefix/kinefix.hpp\"",
        "",
        "namespace Arena3DGenerated",
        "{",
        "    using CollisionBoxRaw = kf_aabb_t;",
        "",
        "    struct WeaponConfigRaw",
        "    {",
        "        int32_t damage; uint32_t cooldownTicks; kf_fixed_t speed; kf_fixed_t launchUpwardSpeed; kf_fixed_t gravity; uint32_t lifetimeTicks;",
        "        uint32_t pelletCount; kf_fixed_t splashRadius; kf_fixed_t knockback; uint32_t ammo; uint32_t chargeTicks;",
        "    };",
        "    struct SpawnRaw { uint32_t id; kf_fixed_t position[3]; uint16_t yaw; };",
        "    struct TurretRaw { uint32_t id; uint32_t weapon; kf_fixed_t position[3]; uint16_t yaw; };",
        "    struct PickupRaw { uint32_t id; uint32_t type; uint32_t weapon; kf_fixed_t position[3]; int32_t amount; };",
        "    struct JumpPadRaw { uint32_t id; kf_fixed_t position[3]; kf_fixed_t halfSize[3]; kf_fixed_t velocity[3]; };",
        "    struct HazardRaw { uint32_t id; uint32_t type; kf_fixed_t position[3]; kf_fixed_t halfSize[3]; int32_t damage; uint32_t intervalTicks; };",
        "",
        "    inline constexpr CollisionBoxRaw CollisionBoxes[] =",
        "    {",
    ]
    for brush_id, center, size in brushes:
        minimum = [fixed(str(center[index] - size[index] * 0.5)) for index in range(3)]
        maximum = [fixed(str(center[index] + size[index] * 0.5)) for index in range(3)]
        values = ", ".join(int32_literal(value) for value in minimum + maximum)
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
        lines.append(f"    inline constexpr kf_fixed_t {cpp_name} = {int32_literal(fixed(movement[json_name]))};")
    arena_half_extent = fixed(str(Decimal(source["level"]["size"][0]) * Decimal("0.5")))
    lines.append(f"    inline constexpr kf_fixed_t ArenaHalfExtent = {int32_literal(arena_half_extent)};")
    lines.extend(["    inline constexpr WeaponConfigRaw WeaponConfigs[] =", "    {"])
    for name in WEAPON_NAMES:
        weapon = source["weapons"][name]
        values = (
            weapon["damage"], weapon["cooldown_ticks"], int32_literal(fixed(weapon["speed"])),
            int32_literal(fixed(weapon.get("launch_upward_speed", "0"))), int32_literal(fixed(weapon["gravity"])), weapon["lifetime_ticks"], weapon["pellet_count"],
            int32_literal(fixed(weapon["splash_radius"])), int32_literal(fixed(weapon["knockback"])),
            weapon["ammo"], weapon.get("charge_ticks", 0),
        )
        lines.append(f"        {{{values[0]}, {values[1]}u, {values[2]}, {values[3]}, {values[4]}, {values[5]}u, {values[6]}u, {values[7]}, {values[8]}, {values[9]}u, {values[10]}u}},")
    lines.extend(["    };", "    inline constexpr SpawnRaw SpawnPoints[] =", "    {"])
    for spawn in source["level"]["spawn_points"]:
        position = ", ".join(int32_literal(fixed(component)) for component in spawn["position"])
        lines.append(f"        {{{spawn['id']}u, {{{position}}}, UINT16_C({fixed_angle(spawn['yaw'])})}},")
    lines.extend(["    };", "    inline constexpr TurretRaw Turrets[] =", "    {"])
    weapon_index = {name: index for index, name in enumerate(WEAPON_NAMES)}
    for turret in source["level"]["turrets"]:
        position = ", ".join(int32_literal(fixed(component)) for component in turret["position"])
        lines.append(f"        {{{turret['id']}u, {weapon_index[turret['archetype']]}u, {{{position}}}, UINT16_C({fixed_angle(turret['yaw'])})}},")
    pickup_type = {"health": 0, "armor": 1, "ammo": 2}
    lines.extend(["    };", "    inline constexpr PickupRaw Pickups[] =", "    {"])
    for pickup in source["level"]["pickups"]:
        position = ", ".join(int32_literal(fixed(component)) for component in pickup["position"])
        pickup_weapon = weapon_index.get(pickup.get("weapon", "nailgun"), 0)
        lines.append(f"        {{{pickup['id']}u, {pickup_type[pickup['type']]}u, {pickup_weapon}u, {{{position}}}, {pickup['amount']}}},")
    lines.extend([
        "    };",
        "    inline constexpr size_t PickupCount = sizeof(Pickups) / sizeof(Pickups[0]);",
        "    inline constexpr JumpPadRaw JumpPads[] =",
        "    {",
    ])
    for pad in source["level"]["jump_pads"]:
        position = ", ".join(int32_literal(fixed(component)) for component in pad["position"])
        half_size = ", ".join(int32_literal(fixed(str(Decimal(component) * Decimal("0.5")))) for component in pad["size"])
        velocity = ", ".join(int32_literal(fixed(component)) for component in pad["velocity"])
        lines.append(f"        {{{pad['id']}u, {{{position}}}, {{{half_size}}}, {{{velocity}}}}},")
    hazard_type = {"lava": 0, "void": 1}
    lines.extend([
        "    };",
        "    inline constexpr size_t JumpPadCount = sizeof(JumpPads) / sizeof(JumpPads[0]);",
        "    inline constexpr HazardRaw Hazards[] =",
        "    {",
    ])
    for hazard in source["level"].get("hazards", []):
        position = ", ".join(int32_literal(fixed(component)) for component in hazard["position"])
        half_size = ", ".join(int32_literal(fixed(str(Decimal(component) * Decimal("0.5")))) for component in hazard["size"])
        lines.append(f"        {{{hazard['id']}u, {hazard_type[hazard['type']]}u, {{{position}}}, {{{half_size}}}, {hazard['damage']}, {hazard['interval_ticks']}u}},")
    lines.extend([
        "    };",
        "    inline constexpr size_t HazardCount = sizeof(Hazards) / sizeof(Hazards[0]);",
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
        payload.extend(struct.pack("<i", fixed(source["movement"][key])))
    for name in WEAPON_NAMES:
        weapon = source["weapons"][name]
        encoded = name.encode("ascii")
        payload.extend(struct.pack("<B", len(encoded)) + encoded)
        for key in sorted(weapon):
            value = weapon[key]
            key_bytes = key.encode("ascii")
            payload.extend(struct.pack("<B", len(key_bytes)) + key_bytes)
            if isinstance(value, int):
                payload.extend(struct.pack("<Bi", 0, value))
            else:
                payload.extend(struct.pack("<Bi", 1, fixed(value)))
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
        payload.extend(struct.pack("<I3iH", spawn["id"], *(fixed(v) for v in spawn["position"]), fixed_angle(spawn["yaw"])))
    payload.extend(struct.pack("<I", len(level["turrets"])))
    for turret in level["turrets"]:
        name = turret["archetype"].encode("ascii")
        payload.extend(struct.pack("<IB", turret["id"], len(name)) + name)
        payload.extend(struct.pack("<3iH", *(fixed(v) for v in turret["position"]), fixed_angle(turret["yaw"])))
    payload.extend(struct.pack("<I", len(level["jump_pads"])))
    for pad in level["jump_pads"]:
        values = pad["position"] + pad["size"] + pad["velocity"]
        payload.extend(struct.pack("<I9i", pad["id"], *(fixed(v) for v in values)))
    hazard_type = {"lava": 0, "void": 1}
    payload.extend(struct.pack("<I", len(level.get("hazards", []))))
    for hazard in level.get("hazards", []):
        values = hazard["position"] + hazard["size"]
        payload.extend(struct.pack("<II6iiI", hazard["id"], hazard_type[hazard["type"]], *(fixed(v) for v in values), hazard["damage"], hazard["interval_ticks"]))
    payload.extend(struct.pack("<I", len(level["pickups"])))
    for pickup in level["pickups"]:
        pickup_type = pickup["type"].encode("ascii")
        weapon = pickup.get("weapon", "").encode("ascii")
        payload.extend(struct.pack("<IB", pickup["id"], len(pickup_type)) + pickup_type)
        payload.extend(struct.pack("<B", len(weapon)) + weapon)
        payload.extend(struct.pack("<3ii", *(fixed(v) for v in pickup["position"]), pickup["amount"]))
    checksum = zlib.crc32(payload) & 0xFFFFFFFF
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<4sIIII", b"A3LV", LEVEL_FORMAT_VERSION, 1, len(payload), checksum) + payload)
    return checksum


def make_textures(output: Path) -> None:
    white = bytes((255, 255, 255, 255)) * (64 * 64)
    write_png(output / "textures" / "white.png", 64, 64, white)

    tile = bytearray()
    stone = bytearray()
    metal = bytearray()
    hazard = bytearray()
    emissive = bytearray()
    decal = bytearray()
    for y in range(64):
        for x in range(64):
            noise = ((x * 17 + y * 29 + (x * y) * 3) % 5) - 2
            seam = x in (0, 1, 32, 33) or y in (0, 1, 32, 33)
            tile_base = 204 if seam else 220 + noise
            tile.extend((tile_base, tile_base + 3, min(255, tile_base + 8), 255))

            block_row = y // 16
            shifted_x = (x + (8 if block_row & 1 else 0)) & 31
            block_y = y & 15
            edge_distance = min(shifted_x, 31 - shifted_x, block_y, 15 - block_y)
            stone_noise = ((x * 13 + y * 23 + x * y * 5) & 15) - 7
            broad_noise = (((x // 4) * 7 + (y // 4) * 11 + block_row * 13) & 7) - 3
            chipped = edge_distance <= 2 and ((x * 5 + y * 7 + block_row * 3) % 19) < 3
            hairline = edge_distance > 3 and ((x * 11 + y * 17 + (x ^ y)) % 113) < 2
            if edge_distance == 0:
                stone_base = 86
            elif edge_distance == 1 or chipped:
                stone_base = 132 + broad_noise
            else:
                stone_base = 188 + stone_noise + broad_noise - (18 if hairline else 0)
            stone.extend((max(0, stone_base - 4), min(255, stone_base + 5), min(255, stone_base + 14), 255))

            panel_seam = x in (0, 1, 31, 32, 62, 63) or y in (0, 1, 31, 32, 62, 63)
            panel_highlight = x in (2, 30, 33, 61) or y in (2, 30, 33, 61)
            rivet_x = x & 31
            rivet_y = y & 31
            rivet = rivet_x in (4, 27) and rivet_y in (4, 27)
            brushed = ((x * 3 + y * 17 + (y // 3) * 5) & 9) - 4
            metal_base = 72 if panel_seam else (184 if panel_highlight else 154 + brushed)
            if rivet:
                metal.extend((220, 230, 238, 255))
            else:
                metal.extend((max(0, metal_base - 7), min(255, metal_base + 7), min(255, metal_base + 18), 255))

            lava_noise = (x * 19 + y * 31 + x * y * 7) & 31
            vein = ((x + y + lava_noise // 4) % 17) < 3
            hazard.extend((255, 208, 42, 255) if vein else (210 + lava_noise, 38 + lava_noise, 4, 255))

            pulse = 18 if ((x // 16) + (y // 16)) & 1 else 0
            emissive.extend((18, 210 + pulse, 190 + pulse, 255))

            dx = (x - 31.5) / 31.5
            dy = (y - 31.5) / 31.5
            radius = math.sqrt(dx * dx + dy * dy)
            irregular_edge = 0.88 + (((x * 13 + y * 7 + x * y) & 15) - 7) * 0.008
            if radius >= irregular_edge:
                decal.extend((18, 14, 12, 0))
            else:
                radial = max(0.0, min(1.0, 1.0 - radius / irregular_edge))
                ring = max(0.0, 1.0 - abs(radius - 0.50) * 5.0)
                crack = ((x * 5 + y * 11 + (x ^ y) * 3) % 41) < 2
                alpha = int(min(230.0, 42.0 + radial * 92.0 + ring * 92.0 + (35.0 if crack else 0.0)))
                soot = int(22.0 + (1.0 - radial) * 18.0)
                decal.extend((soot, max(10, soot - 5), max(8, soot - 8), alpha))
    write_png(output / "textures" / "tile.png", 64, 64, bytes(tile))
    write_png(output / "textures" / "stone.png", 64, 64, bytes(stone))
    write_png(output / "textures" / "metal.png", 64, 64, bytes(metal))
    write_png(output / "textures" / "hazard.png", 64, 64, bytes(hazard))
    write_png(output / "textures" / "emissive.png", 64, 64, bytes(emissive))
    write_png(output / "textures" / "decal.png", 64, 64, bytes(decal))


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

    arena, visual_layers = arena_mesh(source["level"])
    brushes = arena_collision()
    write_glb(output / "models" / "arena01.glb", arena)
    write_glb(output / "models" / "sky.glb", sky_mesh())
    write_glb(output / "models" / "lava.glb", lava_mesh(source["level"]))
    write_glb(output / "models" / "player.glb", player_mesh())
    for layer, chunks in visual_layers.items():
        for index, chunk in enumerate(chunks):
            write_glb(output / "models" / "arena01" / f"{layer}_chunk_{index}.glb", chunk)
    for name in WEAPON_NAMES:
        write_glb(output / "models" / "weapons" / f"{name}.glb", weapon_mesh(name))
    write_glb(output / "models" / "turrets" / "base.glb", turret_mesh("base"))
    for name in WEAPON_NAMES:
        write_glb(output / "models" / "turrets" / f"{name}.glb", turret_mesh(name))
    for name in ("nail", "rocket", "plasma", "grenade", "pellet"):
        write_glb(output / "models" / "projectiles" / f"{name}.glb", projectile_mesh(name))
    for name in ("rocket", "plasma", "grenade"):
        write_glb(output / "models" / "effects" / f"{name}_explosion.glb", explosion_cloud_mesh(name))
    write_glb(output / "models" / "effects" / "impact.glb", impact_flash_mesh())
    write_glb(output / "models" / "effects" / "decal.glb", decal_mesh())
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
        "sky": "models/sky.glb",
        "lava": "models/lava.glb",
        "player": "models/player.glb",
        "impact": "models/effects/impact.glb",
        "decal": "models/effects/decal.glb",
        "chunk_layers": {
            layer: [f"models/arena01/{layer}_chunk_{index}.glb" for index in range(len(chunks))]
            for layer, chunks in visual_layers.items()
        },
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
    for key in ("spawn_points", "turrets", "jump_pads", "hazards", "pickups"):
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
    valid_hazard_types = {"lava", "void"}
    for hazard in source["level"].get("hazards", []):
        if hazard.get("type") not in valid_hazard_types:
            raise ValueError(f"invalid hazard type {hazard['id']}")
        if hazard.get("damage", 0) <= 0 or hazard.get("interval_ticks", 0) <= 0:
            raise ValueError(f"invalid hazard damage cadence {hazard['id']}")
        if any(Decimal(component) <= 0 for component in hazard["size"]):
            raise ValueError(f"invalid hazard size {hazard['id']}")
    level_extent = Decimal(source["level"]["size"][0]) * Decimal("0.5")
    for spawn in source["level"]["spawn_points"]:
        position = [Decimal(component) for component in spawn["position"]]
        if abs(position[0]) >= level_extent or abs(position[2]) >= level_extent or position[1] < 0:
            raise ValueError(f"invalid spawn point {spawn['id']}")
    brushes = arena_collision()
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
    if set(manifest.get("chunk_layers", {})) != set(ARENA_VISUAL_LAYERS):
        raise ValueError("arena visual layers are missing or out of canonical order")
    for layer in ARENA_VISUAL_LAYERS:
        paths = manifest["chunk_layers"][layer]
        if len(paths) != 4 or any(not (output / path).is_file() for path in paths):
            raise ValueError(f"invalid arena visual layer {layer}")
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
