"""Read the bundled Quake III brush map for Arena3D's offline content build."""

from __future__ import annotations

import itertools
import math
import re
import struct
from dataclasses import dataclass, field
from functools import lru_cache
from pathlib import Path


EPSILON = 0.00001
SCALE = 1.0 / 32.0
HEIGHT_OFFSET = 352.0
COLLISION_SLICE = 8.0  # One quarter of an Arena3D unit, below player step height.


def dot(a, b):
    return sum(x * y for x, y in zip(a, b))


def cross(a, b):
    return (a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0])


def subtract(a, b):
    return tuple(x - y for x, y in zip(a, b))


def world(point):
    return (point[0] * SCALE, (point[2] + HEIGHT_OFFSET) * SCALE, -point[1] * SCALE)


@dataclass
class Face:
    normal: tuple[float, float, float]
    distance: float
    texture: str
    mapping: tuple[float, ...]
    vertices: list = field(default_factory=list)


@dataclass
class Brush:
    faces: list[Face] = field(default_factory=list)

    def bounds(self):
        points = [point for face in self.faces for point in face.vertices]
        if not points:
            raise ValueError("empty or unbounded Quake brush")
        return tuple(min(p[i] for p in points) for i in range(3)), tuple(max(p[i] for p in points) for i in range(3))

    def finish(self):
        # Intersection vertices give the original convex brush, including slanted
        # faces. A bounding box alone would fill doorways and bevels with geometry.
        points = set()
        for a, b, c in itertools.combinations(self.faces, 3):
            bc = cross(b.normal, c.normal)
            determinant = dot(a.normal, bc)
            if abs(determinant) < EPSILON:
                continue
            ca = cross(c.normal, a.normal)
            ab = cross(a.normal, b.normal)
            point = tuple((a.distance * bc[i] + b.distance * ca[i] + c.distance * ab[i]) / determinant for i in range(3))
            if all(dot(f.normal, point) <= f.distance + EPSILON for f in self.faces):
                points.add(tuple(round(value, 6) for value in point))
        for face in self.faces:
            polygon = [p for p in sorted(points) if abs(dot(face.normal, p) - face.distance) < EPSILON]
            if len(polygon) < 3:
                continue
            center = tuple(sum(p[i] for p in polygon) / len(polygon) for i in range(3))
            axis = (0, 0, 1) if abs(face.normal[2]) < 0.9 else (0, 1, 0)
            u = cross(axis, face.normal)
            v = cross(face.normal, u)
            face.vertices = sorted(polygon, key=lambda p: math.atan2(dot(subtract(p, center), v), dot(subtract(p, center), u)))
        self.bounds()


@dataclass
class Entity:
    properties: dict[str, str] = field(default_factory=dict)
    brushes: list[Brush] = field(default_factory=list)


@lru_cache(maxsize=4)
def read_map(path: Path) -> list[Entity]:
    entities = []
    depth = 0
    for number, line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = line.strip()
        if not line or line.startswith("//"):
            continue
        if line == "{":
            depth += 1
            if depth == 1:
                entity = Entity()
            elif depth == 2:
                brush = Brush()
            else:
                raise ValueError(f"unsupported nested map primitive at {path}:{number}")
        elif line == "}":
            if depth == 2:
                brush.finish()
                entity.brushes.append(brush)
            elif depth == 1:
                entities.append(entity)
            else:
                raise ValueError(f"unmatched map brace at {path}:{number}")
            depth -= 1
        elif depth == 1 and line.startswith('"'):
            key, value = re.findall(r'"([^\"]*)"', line)
            entity.properties[key] = value
        elif depth == 2 and line.startswith("("):
            a, b, c = [tuple(map(float, p.split())) for p in re.findall(r"\(([^()]*)\)", line)]
            normal = cross(subtract(c, a), subtract(b, a))
            length = math.sqrt(dot(normal, normal))
            if length < EPSILON:
                raise ValueError(f"degenerate map plane at {path}:{number}")
            normal = tuple(value / length for value in normal)
            values = line.rsplit(")", 1)[1].split()
            brush.faces.append(Face(normal, dot(normal, a), values[0], tuple(map(float, values[1:6]))))
        else:
            raise ValueError(f"unsupported map syntax at {path}:{number}")
    if depth or not entities or entities[0].properties.get("classname") != "worldspawn":
        raise ValueError(f"incomplete Quake map: {path}")
    return entities


def texture_uv(face: Face, point, width: int, height: int):
    # Radiant's axial texture projection, before the world coordinate conversion.
    axis = max(range(3), key=lambda i: abs(face.normal[i]))
    u = point[1] if axis == 0 else point[0]
    v = -point[1] if axis == 2 else -point[2]
    shift_u, shift_v, rotation, scale_u, scale_v = face.mapping
    angle = math.radians(rotation)
    rotated_u = math.cos(angle) * u - math.sin(angle) * v
    rotated_v = math.sin(angle) * u + math.cos(angle) * v
    return ((rotated_u / scale_u + shift_u) / width, (rotated_v / scale_v + shift_v) / height)


def clip_polygon(points, axis, boundary, sign):
    clipped = []
    for previous, current in zip(points[-1:] + points[:-1], points):
        a = (previous[axis] - boundary) * sign
        b = (current[axis] - boundary) * sign
        if (a > EPSILON) != (b > EPSILON):
            t = a / (a - b)
            clipped.append(tuple(previous[i] + (current[i] - previous[i]) * t for i in range(3)))
        if b <= EPSILON:
            clipped.append(current)
    return clipped


def collision_boxes(brush: Brush):
    minimum, maximum = brush.bounds()
    diagonal = [f.normal for f in brush.faces if max(map(abs, f.normal)) < 1.0 - EPSILON]
    if not diagonal:
        yield minimum, maximum
        return
    # The simulation accepts AABBs. Slice bevels and ramps instead of blocking
    # the whole brush envelope; the render mesh retains the exact slanted faces.
    axes = [i for i in range(3) if all(abs(n[i]) > EPSILON for n in diagonal)]
    if not axes:
        raise ValueError("Quake brush needs collision decomposition on multiple axes")
    axis = min(axes, key=lambda i: maximum[i] - minimum[i])
    count = max(1, math.ceil((maximum[axis] - minimum[axis]) / COLLISION_SLICE))
    for index in range(count):
        low = minimum[axis] + index * COLLISION_SLICE
        high = min(maximum[axis], low + COLLISION_SLICE)
        points = []
        for face in brush.faces:
            polygon = clip_polygon(face.vertices, axis, low, -1)
            points.extend(clip_polygon(polygon, axis, high, 1))
        if not points:
            continue
        a = tuple(min(p[i] for p in points) for i in range(3))
        b = tuple(max(p[i] for p in points) for i in range(3))
        if all(b[i] - a[i] > EPSILON for i in range(3)):
            yield a, b


def read_tga(path: Path):
    data = path.read_bytes()
    identifier, color_map, image_type = struct.unpack_from("<BBB", data)
    width, height, bits, descriptor = struct.unpack_from("<HHBB", data, 12)
    if color_map or image_type not in (2, 10) or bits not in (24, 32) or descriptor & 16:
        raise ValueError(f"expected RGB(A) TGA: {path}")
    stride = bits // 8
    start = 18 + identifier
    if image_type == 10:
        decoded = bytearray()
        while len(decoded) < width * height * stride:
            if start >= len(data):
                raise ValueError(f"truncated TGA packet: {path}")
            packet = data[start]
            start += 1
            count = (packet & 127) + 1
            size = stride if packet & 128 else count * stride
            if start + size > len(data):
                raise ValueError(f"truncated TGA pixels: {path}")
            decoded.extend(data[start:start + size] * (count if packet & 128 else 1))
            start += size
        if len(decoded) != width * height * stride:
            raise ValueError(f"oversized TGA packet: {path}")
        data, start = decoded, 0
    elif len(data) < start + width * height * stride:
        raise ValueError(f"truncated TGA: {path}")
    pixels = bytearray()
    rows = range(height) if descriptor & 32 else reversed(range(height))
    for y in rows:
        for x in range(width):
            offset = start + (y * width + x) * stride
            blue, green, red = data[offset:offset + 3]
            alpha = data[offset + 3] if stride == 4 else 255
            pixels.extend((red, green, blue, alpha))
    return width, height, bytes(pixels)
