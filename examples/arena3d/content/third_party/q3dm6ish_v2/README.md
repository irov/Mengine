# q3dm6ish_v2

This directory contains the download manifest and attribution for GPL-2.0-or-later
level content, separate from the engine's own source license. CMake downloads
the original files into `resources/.downloads/q3dm6ish_v2` and checks SHA-256.
Keep these notices and the conversion tools with generated level distributions.

- Map: `q3dm6ish_v2`, a retexture of aardappel's q3dm6ish by TRaK.
- Brushwork and entity placement: aardappel and leilol, modified by TRaK;
  the original notice marks leilol's attribution as uncertain.
- Textures: Georges "TRaK" Grondin, TRaK2 texture set.
- Map source and author notice:
  https://openarena.ws/svn/source/assets/maps/q3dm6ish_v2.map
  https://openarena.ws/svn/source/assets/maps/q3dm6ish_v2_readme.txt
- The fourteen original TGA textures referenced by this map were recovered
  from OpenGameArt's TRaK2 collection. Per-file URLs and SHA-256 hashes are in
  `sources.json`. Example texture page with author/license information:
  https://opengameart.org/content/dirty-grey-wall-4-rows
- TRaK2 is available under GPL 2.0, GPL 3.0, or CC-BY-SA 3.0. These copies are
  used under GPL 2.0. Some source textures are copyrighted by CGTextures.com;
  TRaK received permission to release these specific derived textures under
  GPL/CC-BY-SA. That permission does not cover arbitrary CGTextures assets.

`q3dm6ish_v2.map`, `README.original.txt`, and the TGA files are unmodified.
`COPYING` contains GPL version 2 from the same OpenArena map source directory.
The original readme refers to a GPLv3 attachment, but explicitly permits GPLv2
or later. Original texture source files were also distributed by TRaK as
`trak2-source.zip`: https://freegamearts.tuxfamily.org/fga-item.php?name=TRaK2+texture+set

The unmodified compiled BSP was recovered from OpenArena revision 888:
https://openarena.ws/svn/!svn/bc/888/maps/q3dm6ish_v2.bsp
Its 2,049 polygon surfaces and 76 RGB lightmaps provide the rendered geometry,
UVs, baked illumination and shadows. `trak2.shader` is retained as the source
reference for emissive panels and the warm sun (1, 0.86, 0.48).

Arena3D converts BSP vertices and both UV sets into GLB. Each material gets a
single PNG atlas: a 510-pixel repeating diffuse tile with a wrapped gutter,
followed by that material's original 128-pixel lightmap pages. The example
shader samples both regions and applies Quake-style 2x lightmap overbright.
Light panels retain a fullbright contribution. Postprocessing applies a 1.3
display gamma without an additional HDR tone map. These are explicit rendering
adaptations; arbitrary Quake shader stages are not executed.

Coordinates use 32 Quake units per engine unit, Z-up to Y-up, and a vertical
offset of 352 Quake units. Collision still comes from the source map brushes,
with diagonal brushes sliced into AABBs at intervals of at most 8 Quake units.
This approximates collision for the existing controller; it does not implement
Quake BSP collision. The generated sky is retained. Original item/spawn
positions and the three jump-pad routes are adapted to the existing gameplay
slots in `arena3d_source.json`; the main-hall spawn is shown first.
