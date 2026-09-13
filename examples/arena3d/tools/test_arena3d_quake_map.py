import struct
import tempfile
import unittest
from pathlib import Path

import arena3d_quake_map as quake
import arena3d_quake_assets as assets


def cube():
    faces = []
    for axis in range(3):
        for sign in (-1, 1):
            normal = tuple(sign if i == axis else 0 for i in range(3))
            faces.append(quake.Face(normal, 0 if sign == -1 else 16, "test", (0, 0, 0, 1, 1)))
    return quake.Brush(faces)


class QuakeMapTests(unittest.TestCase):
    def test_original_bsp_contains_geometry_and_lightmaps(self):
        root = Path(__file__).resolve().parents[1] / "resources" / ".downloads"
        bsp = assets.read_bsp(root / "q3dm6ish_v2" / "q3dm6ish_v2.bsp")
        self.assertEqual(len(bsp.lightmaps), 76)
        self.assertEqual(len(bsp.faces), 2049)
        self.assertTrue(all(len(page) == 128 * 128 * 3 for page in bsp.lightmaps))
        self.assertTrue(any(v[3:5] != v[5:7] for v in bsp.vertices))

    def test_weapon_barrel_and_muzzle_tags(self):
        root = Path(__file__).resolve().parents[1] / "resources" / ".downloads" / "openarena_models"
        surfaces, tags = assets.read_md3(root / "models/weapons2/machinegun/machinegun.md3")
        self.assertEqual(len(surfaces), 2)
        self.assertGreater(tags["tag_flash"][0][2], tags["tag_barrel"][0][2])
        barrel, _ = assets.read_md3(root / "models/weapons2/machinegun/machinegun_barrel.md3")
        # MD3 and BSP have opposite winding conventions. Preserve MD3 winding.
        for triangle in barrel[0].triangles:
            a, b, c = (barrel[0].positions[i] for i in triangle)
            normal = quake.cross(quake.subtract(b, a), quake.subtract(c, a))
            self.assertGreater(quake.dot(normal, barrel[0].normals[triangle[0]]), 0)

    def test_rle_tga_packets_and_truncation(self):
        header = struct.pack("<BBBHHBHHHHBB", 0, 0, 10, 0, 0, 0, 0, 0, 3, 1, 24, 32)
        # Two red pixels in a run, followed by a raw blue pixel.
        packets = bytes((129, 0, 0, 255, 0, 255, 0, 0))
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "rle.tga"
            path.write_bytes(header + packets)
            self.assertEqual(quake.read_tga(path)[2], bytes((255, 0, 0, 255)) * 2 + bytes((0, 0, 255, 255)))
            path.write_bytes(header + packets[:-1])
            with self.assertRaises(ValueError):
                quake.read_tga(path)

    def test_brush_winding_survives_axis_conversion(self):
        brush = cube()
        brush.finish()
        self.assertEqual(brush.bounds(), ((0, 0, 0), (16, 16, 16)))
        self.assertEqual(list(quake.collision_boxes(brush)), [brush.bounds()])
        for face in brush.faces:
            a, b, c = map(quake.world, face.vertices[:3])
            normal = quake.cross(quake.subtract(b, a), quake.subtract(c, a))
            expected = (face.normal[0], face.normal[2], -face.normal[1])
            self.assertGreater(quake.dot(normal, expected), 0)

    def test_bevel_collision_does_not_fill_brush_envelope(self):
        brush = cube()
        root_two = 2 ** 0.5
        brush.faces.append(quake.Face((1 / root_two, 1 / root_two, 0), 16 / root_two, "test", (0, 0, 0, 1, 1)))
        brush.finish()
        boxes = list(quake.collision_boxes(brush))
        self.assertGreater(len(boxes), 1)
        self.assertFalse(any(all(a[i] <= (15, 15, 8)[i] <= b[i] for i in range(3)) for a, b in boxes))
        for x in range(1, 16):
            for y in range(1, 16 - x):
                self.assertTrue(any(all(a[i] <= (x, y, 8)[i] <= b[i] for i in range(3)) for a, b in boxes))

    def test_texture_projection_in_source_coordinates(self):
        face = quake.Face((0, 0, 1), 0, "test", (16, 32, 0, 0.5, 0.25))
        self.assertEqual(quake.texture_uv(face, (64, 32, 0), 256, 128), (0.5625, -0.75))
        face.mapping = (0, 0, 90, 1, 1)
        u, v = quake.texture_uv(face, (64, 32, 0), 256, 128)
        self.assertAlmostEqual(u, 0.125)
        self.assertAlmostEqual(v, 0.5)

    def test_tga_origin_and_channel_order(self):
        # Bottom row blue/white, top row red/green, stored as BGR.
        header = struct.pack("<BBBHHBHHHHBB", 0, 0, 2, 0, 0, 0, 0, 0, 2, 2, 24, 0)
        pixels = bytes((255, 0, 0, 255, 255, 255, 0, 0, 255, 0, 255, 0))
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "test.tga"
            path.write_bytes(header + pixels)
            width, height, decoded = quake.read_tga(path)
            self.assertEqual((width, height), (2, 2))
            self.assertEqual(decoded, bytes((255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255, 255)))
            path.write_bytes(header + pixels[:-1])
            with self.assertRaises(ValueError):
                quake.read_tga(path)


if __name__ == "__main__":
    unittest.main()
