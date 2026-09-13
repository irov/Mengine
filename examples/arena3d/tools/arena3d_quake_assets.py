"""Offline readers for the bundled OpenArena BSP46 and MD3 assets.

Format reference: id Software's code/qcommon/qfiles.h. No game code executes
during conversion. Only polygon BSP surfaces and the first MD3 frame are used;
weapon tags are retained for rigid barrel and muzzle animation in Mengine.
"""

from dataclasses import dataclass
from pathlib import Path
import math
import struct


def records(data: bytes, offset: int, length: int, layout: str):
    size = struct.calcsize(layout)
    if offset < 0 or length < 0 or offset + length > len(data) or length % size:
        raise ValueError("invalid Quake asset section")
    return list(struct.iter_unpack(layout, data[offset:offset + length]))


def text(value: bytes) -> str:
    return value.split(b"\0", 1)[0].decode("ascii").replace("\\", "/")


@dataclass
class Bsp:
    shaders: list
    vertices: list
    indices: list
    faces: list
    lightmaps: list[bytes]


def read_bsp(path: Path) -> Bsp:
    data = path.read_bytes()
    if data[:8] != struct.pack("<4si", b"IBSP", 46):
        raise ValueError("expected Quake III BSP version 46")
    lumps = records(data, 8, 17 * 8, "<2i")
    shaders = records(data, *lumps[1], "<64s2i")
    vertices = records(data, *lumps[10], "<10f4B")
    indices = records(data, *lumps[11], "<i")
    faces = records(data, *lumps[13], "<12i12f2i")
    lightmaps = records(data, *lumps[14], "<49152s")
    for face in faces:
        shader, _, kind, start, count, first, size = face[:7]
        if kind != 1 or shader < 0 or shader >= len(shaders):
            raise ValueError("expected polygon BSP surfaces")
        if start < 0 or start + count > len(vertices) or first < 0 or first + size > len(indices):
            raise ValueError("invalid BSP surface range")
        if size % 3 or any(i[0] < 0 or i[0] >= count for i in indices[first:first + size]):
            raise ValueError("invalid BSP triangle index")
        if face[7] >= len(lightmaps):
            raise ValueError("invalid BSP lightmap index")
    return Bsp([text(s[0]) for s in shaders], vertices, [i[0] for i in indices], faces, [p[0] for p in lightmaps])


@dataclass
class Md3Surface:
    shader: str
    positions: list
    normals: list
    uvs: list
    triangles: list


def model_point(point):
    return (-point[1] / 32.0, point[2] / 32.0, point[0] / 32.0)


def read_md3(path: Path):
    data = path.read_bytes()
    header = records(data, 0, 108, "<4si64s9i")[0]
    if header[:2] != (b"IDP3", 15) or header[4] < 1 or header[11] != len(data):
        raise ValueError("expected complete MD3 version 15")
    tags = {}
    for tag in records(data, header[9], header[5] * 112, "<64s12f"):
        tags[text(tag[0])] = (model_point(tag[1:4]), tag[4:])
    surfaces = []
    offset = header[10]
    for _ in range(header[6]):
        h = records(data, offset, 108, "<4s64s10i")[0]
        _, _, _, frames, shaders, count, triangles, tri_offset, shader_offset, uv_offset, vertex_offset, end = h
        if h[0] != b"IDP3" or frames < 1 or shaders < 1 or end < 108 or offset + end > len(data):
            raise ValueError("invalid MD3 surface")
        shader = records(data, offset + shader_offset, 68, "<64si")[0]
        vertices = records(data, offset + vertex_offset, count * 8, "<3h2B")
        uvs = records(data, offset + uv_offset, count * 8, "<2f")
        indices = records(data, offset + tri_offset, triangles * 12, "<3i")
        if any(i < 0 or i >= count for tri in indices for i in tri):
            raise ValueError("invalid MD3 triangle index")
        normals = []
        for v in vertices:
            latitude, longitude = v[4] * math.tau / 255, v[3] * math.tau / 255
            normal = (math.cos(latitude) * math.sin(longitude), math.sin(latitude) * math.sin(longitude), math.cos(longitude))
            normals.append((-normal[1], normal[2], normal[0]))
        positions = [model_point(tuple(c / 64.0 for c in v[:3])) for v in vertices]
        surfaces.append(Md3Surface(text(shader[0]), positions, normals, uvs, indices))
        offset += end
    return surfaces, tags
