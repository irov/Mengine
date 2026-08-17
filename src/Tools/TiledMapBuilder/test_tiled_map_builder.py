import base64
import json
import struct
import tempfile
import unittest
import zlib
from pathlib import Path

import tiled_map_builder


class TiledMapBuilderTest(unittest.TestCase):
    def test_multilayer_external_tileset_and_objects(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            tileset_path = root / "terrain.tsj"
            map_path = root / "level.tmj"
            tileset_path.write_text(
                json.dumps(
                    {
                        "name": "Terrain",
                        "tilewidth": 32,
                        "tileheight": 32,
                        "tilecount": 4,
                        "columns": 2,
                        "image": "terrain.png",
                        "imagewidth": 64,
                        "imageheight": 64,
                        "properties": [
                            {"name": "mengine_resource", "type": "string", "value": "TerrainAtlas"}
                        ],
                    }
                ),
                encoding="utf-8",
            )
            map_data = {
                "orientation": "orthogonal",
                "renderorder": "right-down",
                "width": 2,
                "height": 2,
                "tilewidth": 32,
                "tileheight": 32,
                "properties": [{"name": "night", "type": "bool", "value": True}],
                "tilesets": [{"firstgid": 1, "source": "terrain.tsj"}],
                "layers": [
                    {
                        "id": 1,
                        "name": "Ground",
                        "type": "tilelayer",
                        "width": 2,
                        "height": 2,
                        "data": [1, 0x80000002, 0, 3],
                    },
                    {
                        "id": 2,
                        "name": "Gameplay",
                        "type": "group",
                        "offsetx": 4,
                        "offsety": 6,
                        "layers": [
                            {
                                "id": 3,
                                "name": "Infinite",
                                "type": "tilelayer",
                                "chunks": [{"x": -2, "y": 3, "width": 2, "height": 1, "data": [4, 0]}],
                            },
                            {
                                "id": 4,
                                "name": "Objects",
                                "type": "objectgroup",
                                "properties": [{"name": "kind", "type": "string", "value": "spawn"}],
                                "objects": [
                                    {
                                        "id": 11,
                                        "name": "Route",
                                        "class": "Patrol",
                                        "x": 10,
                                        "y": 20,
                                        "polyline": [{"x": 0, "y": 0}, {"x": 12, "y": 4}],
                                        "properties": [{"name": "speed", "type": "float", "value": 1.5}],
                                    }
                                ],
                            },
                        ],
                    },
                ],
            }
            map_path.write_text(json.dumps(map_data), encoding="utf-8")

            output = tiled_map_builder.convert(map_path)
            self.assertEqual(output["Orientation"], tiled_map_builder.ORIENTATIONS["orthogonal"])
            self.assertEqual(output["Width"], 2)
            self.assertEqual(output["Height"], 2)
            self.assertEqual(output["Property"], [{"Name": "night", "Value": "true", "Type": 1}])
            self.assertEqual(output["Tileset"][0]["Resource"], "TerrainAtlas")
            self.assertEqual(
                [layer["Name"] for layer in output["TileLayer"]],
                ["Ground", "Gameplay/Infinite"],
            )
            self.assertEqual(output["TileLayer"][1]["Tile"], [{"Position": [-2, 3], "Gid": 4}])
            self.assertEqual(output["ObjectLayer"][0]["Name"], "Gameplay/Objects")
            self.assertEqual(output["ObjectLayer"][0]["Object"][0]["Class"], "Patrol")
            self.assertEqual(output["ObjectLayer"][0]["Object"][0]["Position"], [14.0, 26.0])
            self.assertEqual(
                output["ObjectLayer"][0]["Object"][0]["Point"],
                [{"Position": [0.0, 0.0]}, {"Position": [12.0, 4.0]}],
            )
            self.assertEqual(output, tiled_map_builder.convert(map_path))

            tilesets = tiled_map_builder.normalize_tilesets(map_data, map_path)
            self.assertEqual(tilesets[0]["resource"], "TerrainAtlas")
            tile_layers, object_layers = tiled_map_builder.collect_layers(map_data)
            self.assertEqual([layer["name"] for layer in tile_layers], ["Ground", "Gameplay/Infinite"])
            self.assertEqual(tile_layers[1]["tiles"], [(-2, 3, 4)])
            self.assertEqual(object_layers[0]["name"], "Gameplay/Objects")
            self.assertEqual(object_layers[0]["objects"][0]["class"], "Patrol")
            self.assertEqual(object_layers[0]["objects"][0]["x"], 14.0)
            self.assertEqual(object_layers[0]["objects"][0]["points"], [(0.0, 0.0), (12.0, 4.0)])

    def test_zlib_base64_layer(self):
        gids = [1, 0x40000002, 0, 3]
        raw = struct.pack("<4I", *gids)
        layer = {
            "encoding": "base64",
            "compression": "zlib",
            "data": base64.b64encode(zlib.compress(raw)).decode("ascii"),
        }

        self.assertEqual(tiled_map_builder.decode_layer_data(layer, 4), gids)

    def test_rejects_unsupported_orientation(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            map_path = Path(temporary_directory) / "level.tmj"
            map_path.write_text(json.dumps({"orientation": "staggered"}), encoding="utf-8")

            with self.assertRaisesRegex(ValueError, "unsupported Tiled orientation"):
                tiled_map_builder.convert(map_path)

    def test_rejects_unsupported_property_type(self):
        properties = [{"name": "config", "type": "class", "value": {}}]

        with self.assertRaisesRegex(ValueError, "unsupported Tiled property type"):
            tiled_map_builder.normalize_properties(properties)


if __name__ == "__main__":
    unittest.main()
