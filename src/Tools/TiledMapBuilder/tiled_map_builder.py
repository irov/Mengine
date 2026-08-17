#!/usr/bin/env python3
"""Convert Tiled JSON/TSJ maps to the Mengine TiledMap Metabuf schema."""

from __future__ import annotations

import argparse
import base64
import gzip
import json
import struct
import sys
import zlib
from pathlib import Path
from typing import Any, Iterable


ORIENTATIONS = {"orthogonal": 0, "isometric": 1, "hexagonal": 2}
STAGGER_AXIS = {"x": 0, "y": 1}
STAGGER_INDEX = {"even": 0, "odd": 1}

PROPERTY_TYPES = {
    "string": 0,
    "bool": 1,
    "int": 2,
    "float": 3,
    "color": 4,
    "file": 5,
    "object": 6,
}

OBJECT_SHAPES = {
    "rectangle": 0,
    "ellipse": 1,
    "point": 2,
    "polygon": 3,
    "polyline": 4,
    "tile": 5,
    "text": 6,
}


def load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as stream:
        value = json.load(stream)

    if not isinstance(value, dict):
        raise ValueError(f"{path}: root must be an object")

    return value


def find_property(owner: dict[str, Any], name: str, default: Any = None) -> Any:
    for prop in owner.get("properties", []):
        if prop.get("name") == name:
            return prop.get("value", default)

    return default


def property_value(prop: dict[str, Any]) -> str:
    value = prop.get("value", "")
    prop_type = prop.get("type", "string")

    if prop_type == "bool":
        return "true" if bool(value) else "false"

    if value is None:
        return ""

    if isinstance(value, (dict, list)):
        return json.dumps(value, separators=(",", ":"), ensure_ascii=False)

    return str(value)


def normalize_properties(properties: Iterable[dict[str, Any]]) -> list[dict[str, Any]]:
    result = []

    for prop in properties:
        prop_type = str(prop.get("type", "string"))

        if prop_type not in PROPERTY_TYPES:
            raise ValueError(f"unsupported Tiled property type: {prop_type}")

        result.append(
            {
                "Name": str(prop.get("name", "")),
                "Value": property_value(prop),
                "Type": PROPERTY_TYPES[prop_type],
            }
        )

    return result


def image_resource_name(owner: dict[str, Any], image: str) -> str:
    explicit = find_property(owner, "mengine_resource")

    if explicit:
        return str(explicit)

    return Path(image).stem


def decode_layer_data(layer: dict[str, Any], expected_count: int) -> list[int]:
    data = layer.get("data", [])

    if isinstance(data, list):
        result = [int(value) & 0xFFFFFFFF for value in data]
    elif isinstance(data, str):
        raw = base64.b64decode(data.strip())
        compression = layer.get("compression", "")

        if compression == "zlib":
            raw = zlib.decompress(raw)
        elif compression == "gzip":
            raw = gzip.decompress(raw)
        elif compression == "zstd":
            try:
                import zstandard  # type: ignore
            except ImportError as exc:
                raise ValueError("zstd-compressed Tiled layer requires the 'zstandard' package") from exc

            raw = zstandard.ZstdDecompressor().decompress(raw)
        elif compression:
            raise ValueError(f"unsupported Tiled compression: {compression}")

        if len(raw) % 4 != 0:
            raise ValueError("base64 tile layer byte count is not divisible by four")

        result = list(struct.unpack(f"<{len(raw) // 4}I", raw))
    else:
        raise ValueError("tile layer data must be an array or base64 string")

    if len(result) != expected_count:
        raise ValueError(f"tile layer contains {len(result)} gids, expected {expected_count}")

    return result


def load_tileset(entry: dict[str, Any], map_path: Path) -> tuple[int, dict[str, Any], Path]:
    first_gid = int(entry.get("firstgid", 0))
    source = entry.get("source")

    if source:
        source_path = (map_path.parent / str(source)).resolve()

        if source_path.suffix.lower() not in (".json", ".tsj"):
            raise ValueError(f"{source_path}: only JSON/TSJ external tilesets are supported")

        return first_gid, load_json(source_path), source_path

    return first_gid, entry, map_path


def normalize_tilesets(map_data: dict[str, Any], map_path: Path) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []

    for entry in map_data.get("tilesets", []):
        first_gid, tileset, source_path = load_tileset(entry, map_path)
        tile_width = int(tileset.get("tilewidth", map_data.get("tilewidth", 0)))
        tile_height = int(tileset.get("tileheight", map_data.get("tileheight", 0)))
        columns = int(tileset.get("columns", 0))
        margin = int(tileset.get("margin", 0))
        spacing = int(tileset.get("spacing", 0))
        image = str(tileset.get("image", ""))
        image_width = int(tileset.get("imagewidth", 0))
        image_height = int(tileset.get("imageheight", 0))
        tile_count = int(tileset.get("tilecount", 0))

        if tile_count == 0 and columns and tile_height and image_height:
            rows = max((image_height - margin * 2 + spacing) // (tile_height + spacing), 0)
            tile_count = columns * rows

        tile_images: list[dict[str, Any]] = []
        maximum_local_id = -1

        for tile in tileset.get("tiles", []):
            local_id = int(tile.get("id", 0))
            maximum_local_id = max(maximum_local_id, local_id)
            tile_image = str(tile.get("image", ""))

            if not tile_image:
                continue

            tile_images.append(
                {
                    "local_id": local_id,
                    "resource": image_resource_name(tile, tile_image),
                    "image_width": int(tile.get("imagewidth", tile_width)),
                    "image_height": int(tile.get("imageheight", tile_height)),
                }
            )

        tile_count = max(tile_count, maximum_local_id + 1)

        if tile_count <= 0:
            raise ValueError(f"{source_path}: tileset '{tileset.get('name', '')}' has no tiles")

        tile_offset = tileset.get("tileoffset", {})
        result.append(
            {
                "first_gid": first_gid,
                "tile_count": tile_count,
                "columns": columns,
                "tile_width": tile_width,
                "tile_height": tile_height,
                "margin": margin,
                "spacing": spacing,
                "image_width": image_width,
                "image_height": image_height,
                "offset_x": int(tile_offset.get("x", 0)),
                "offset_y": int(tile_offset.get("y", 0)),
                "name": str(tileset.get("name", "")),
                "resource": image_resource_name(tileset, image) if image else "",
                "tile_images": tile_images,
            }
        )

    result.sort(key=lambda value: value["first_gid"])
    return result


def merge_properties(parent: list[dict[str, Any]], child: list[dict[str, Any]]) -> list[dict[str, Any]]:
    merged = {str(prop.get("name", "")): prop for prop in parent}
    merged.update({str(prop.get("name", "")): prop for prop in child})
    return list(merged.values())


def collect_layers(map_data: dict[str, Any]) -> tuple[list[dict[str, Any]], list[dict[str, Any]]]:
    tile_layers: list[dict[str, Any]] = []
    object_layers: list[dict[str, Any]] = []
    render_order = str(map_data.get("renderorder", "right-down"))

    def visit(
        layers: Iterable[dict[str, Any]],
        prefix: str,
        parent_visible: bool,
        parent_opacity: float,
        parent_offset_x: float,
        parent_offset_y: float,
        parent_properties: list[dict[str, Any]],
    ) -> None:
        for layer in layers:
            layer_type = layer.get("type")
            name = str(layer.get("name", ""))
            full_name = f"{prefix}/{name}" if prefix and name else prefix or name
            visible = parent_visible and bool(layer.get("visible", True))
            opacity = parent_opacity * float(layer.get("opacity", 1.0))
            offset_x = parent_offset_x + float(layer.get("offsetx", 0.0))
            offset_y = parent_offset_y + float(layer.get("offsety", 0.0))
            properties = merge_properties(parent_properties, list(layer.get("properties", [])))

            if layer_type == "group":
                visit(layer.get("layers", []), full_name, visible, opacity, offset_x, offset_y, properties)
                continue

            base = {
                "id": int(layer.get("id", 0)),
                "name": full_name,
                "visible": visible,
                "opacity": opacity,
                "offset_x": offset_x,
                "offset_y": offset_y,
                "properties": properties,
            }

            if layer_type == "tilelayer":
                tiles: list[tuple[int, int, int]] = []
                layer_x = int(layer.get("x", layer.get("startx", 0)))
                layer_y = int(layer.get("y", layer.get("starty", 0)))

                if layer.get("chunks") is not None:
                    for chunk in layer.get("chunks", []):
                        width = int(chunk.get("width", 0))
                        height = int(chunk.get("height", 0))
                        gids = decode_layer_data(chunk, width * height)
                        start_x = int(chunk.get("x", 0)) + layer_x
                        start_y = int(chunk.get("y", 0)) + layer_y

                        for index, gid in enumerate(gids):
                            if gid:
                                tiles.append((start_x + index % width, start_y + index // width, gid))
                else:
                    width = int(layer.get("width", map_data.get("width", 0)))
                    height = int(layer.get("height", map_data.get("height", 0)))
                    gids = decode_layer_data(layer, width * height)

                    for index, gid in enumerate(gids):
                        if gid:
                            tiles.append((layer_x + index % width, layer_y + index // width, gid))

                reverse_x = render_order.startswith("left")
                reverse_y = render_order.endswith("up")
                tiles.sort(key=lambda tile: ((-tile[1] if reverse_y else tile[1]), (-tile[0] if reverse_x else tile[0])))
                base["tiles"] = tiles
                tile_layers.append(base)
                continue

            if layer_type == "objectgroup":
                objects: list[dict[str, Any]] = []

                for obj in layer.get("objects", []):
                    shape = "rectangle"
                    points: list[tuple[float, float]] = []

                    for candidate in ("ellipse", "point", "polygon", "polyline", "text"):
                        if candidate in obj and obj.get(candidate) is not False:
                            shape = candidate
                            break

                    if "gid" in obj:
                        shape = "tile"

                    if shape in ("polygon", "polyline"):
                        points = [(float(point.get("x", 0.0)), float(point.get("y", 0.0))) for point in obj.get(shape, [])]

                    objects.append(
                        {
                            "id": int(obj.get("id", 0)),
                            "name": str(obj.get("name", "")),
                            "type": str(obj.get("type", "")),
                            "class": str(obj.get("class", obj.get("type", ""))),
                            "shape": OBJECT_SHAPES[shape],
                            "visible": bool(obj.get("visible", True)),
                            "x": float(obj.get("x", 0.0)) + offset_x,
                            "y": float(obj.get("y", 0.0)) + offset_y,
                            "width": float(obj.get("width", 0.0)),
                            "height": float(obj.get("height", 0.0)),
                            "rotation": float(obj.get("rotation", 0.0)),
                            "gid": int(obj.get("gid", 0)) & 0xFFFFFFFF,
                            "properties": list(obj.get("properties", [])),
                            "points": points,
                        }
                    )

                base["objects"] = objects
                object_layers.append(base)

    visit(map_data.get("layers", []), "", True, 1.0, 0.0, 0.0, [])
    return tile_layers, object_layers


def normalize_layer(layer: dict[str, Any]) -> dict[str, Any]:
    return {
        "Id": layer["id"],
        "Name": layer["name"],
        "Visible": layer["visible"],
        "Opacity": layer["opacity"],
        "Offset": [layer["offset_x"], layer["offset_y"]],
        "Property": normalize_properties(layer["properties"]),
    }


def convert(input_path: Path) -> dict[str, Any]:
    map_data = load_json(input_path)
    orientation_name = str(map_data.get("orientation", "orthogonal"))

    if orientation_name not in ORIENTATIONS:
        raise ValueError(f"unsupported Tiled orientation: {orientation_name}")

    tilesets = normalize_tilesets(map_data, input_path)
    tile_layers, object_layers = collect_layers(map_data)
    result: dict[str, Any] = {
        "Orientation": ORIENTATIONS[orientation_name],
        "StaggerAxis": STAGGER_AXIS.get(str(map_data.get("staggeraxis", "x")), 0),
        "StaggerIndex": STAGGER_INDEX.get(str(map_data.get("staggerindex", "even")), 0),
        "Width": int(map_data.get("width", 0)),
        "Height": int(map_data.get("height", 0)),
        "TileWidth": int(map_data.get("tilewidth", 0)),
        "TileHeight": int(map_data.get("tileheight", 0)),
        "HexSideLength": int(map_data.get("hexsidelength", 0)),
        "Property": normalize_properties(map_data.get("properties", [])),
        "Tileset": [],
        "TileLayer": [],
        "ObjectLayer": [],
    }

    for tileset in tilesets:
        result["Tileset"].append(
            {
                "FirstGid": tileset["first_gid"],
                "TileCount": tileset["tile_count"],
                "Columns": tileset["columns"],
                "TileWidth": tileset["tile_width"],
                "TileHeight": tileset["tile_height"],
                "Margin": tileset["margin"],
                "Spacing": tileset["spacing"],
                "ImageWidth": tileset["image_width"],
                "ImageHeight": tileset["image_height"],
                "Offset": [tileset["offset_x"], tileset["offset_y"]],
                "Name": tileset["name"],
                "Resource": tileset["resource"],
                "Image": [
                    {
                        "LocalId": image["local_id"],
                        "Resource": image["resource"],
                        "Width": image["image_width"],
                        "Height": image["image_height"],
                    }
                    for image in tileset["tile_images"]
                ],
            }
        )

    for layer in tile_layers:
        value = normalize_layer(layer)
        value["Tile"] = [
            {"Position": [x, y], "Gid": gid}
            for x, y, gid in layer["tiles"]
        ]
        result["TileLayer"].append(value)

    for layer in object_layers:
        value = normalize_layer(layer)
        value["Object"] = [
            {
                "Id": obj["id"],
                "Name": obj["name"],
                "Type": obj["type"],
                "Class": obj["class"],
                "Shape": obj["shape"],
                "Visible": obj["visible"],
                "Position": [obj["x"], obj["y"]],
                "Size": [obj["width"], obj["height"]],
                "Rotation": obj["rotation"],
                "Gid": obj["gid"],
                "Property": normalize_properties(obj["properties"]),
                "Point": [{"Position": [x, y]} for x, y in obj["points"]],
            }
            for obj in layer["objects"]
        ]
        result["ObjectLayer"].append(value)

    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", type=Path, help="Tiled .json/.tmj map")
    parser.add_argument("output", type=Path, help="Mengine Metabuf .json output")
    args = parser.parse_args()

    try:
        output = convert(args.input.resolve())
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(output, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(f"tiled_map_builder: {exc}", file=sys.stderr)
        return 1

    print(args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
