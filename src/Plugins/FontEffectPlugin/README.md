# FontEffectPlugin

`FontEffectPlugin` is a native, software-rasterized font effect service: fill (solid or gradient), outline, shadow, glow, inner shadow, inner glow, bevel/emboss and blur, stacked as up to `MENGINE_FONTEFFECT_MAX_LAYERS` (4) layers, each layer producing one of `TTFGlyph::quads[4]`. It plays the same role as `FEPlugin` (`dependencies/libfe`) but does not use FE's FEF2 format or its own rasterizer; the two are independent and a font may use one or the other, never both.

An effect reaches a `TTFFont` through `FontInterface::setEffect(FontEffectInterfacePtr)`, which must be called before the font is compiled (`TTFFont::setEffect` asserts `isCompile() == false`).

There is exactly one effect class, `FontEffectBase`, and its own API is how it gets configured: `addLayer()`, `addEffect(layer, type)` and one setter per field (`setEffectColor`, `setEffectWidth`, `setEffectBlur`, `addEffectGradientStop`, ...). **Every setter validates its value at the moment it is set** and returns `false` + logs on a bad one (negative size, opacity below zero, a fifth layer past `MENGINE_FONTEFFECT_MAX_LAYERS`, unsorted gradient stops, an out-of-range type). There is no separate "validate the whole desc afterwards" pass — whatever is inside an effect is valid by construction.

The loader is just another caller of that same API: `FontEffectFile` (a thin subclass that adds content + effect name) reads a presets JSON file through the engine's standard `Resource`/`Dataflow` mechanism (`Helper::getDataflow`, vocabulary name `"fontEffect"` — the same pattern `FEPlugin` uses for FEF2 bundles) and replays the parsed entry into itself field by field through those setters.

Two `FontEffectServiceInterface` factories wrap this:

- `createFontEffectFromFile(content, effectName, sample, doc)` — file-backed effect (`UnknownFontEffectFileInterface`, declared in the engine `Interface/FontEffectInterface.h` and shared with `FEPlugin`). Wired into `Fonts.json` via `FEType: "FontEffectFile"` + `FEPath` + `FEName` (+ optional `FESample`).
- `createFontEffect(desc, sample, doc)` — convenience for game code that already has a `FontEffectDesc` in hand: it walks the struct and drives the same builder API, returning `nullptr` if any field is rejected. JSON parsing only ever happens inside the `fontEffect` dataflow; there is no API that turns bare JSON into an effect without going through a presets file.

`Fonts.json` uses one set of keys for both effect backends; `FEType` names the prototype in the `"FontEffect"` category and selects which one is used — `"FEFile"` for `FEPlugin`'s FEF2 bundles (the default when `FEType` is omitted) or `"FontEffectFile"` for this plugin. The config loader (`TTFFontConfigLoader.cpp`) knows about neither plugin: it only generates the prototype and hands it the content, and each effect attaches its own dataflow in `setContent`.

```json
"Title": { "Type": "TTF", "Glyph": "RobotoBold", "Height": 48, "FEType": "FontEffectFile", "FEPath": "Fonts/Effects.json", "FEName": "GoldTitle", "FESample": 2 }
```

`FESample`/`sample` is a supersample multiplier (glyphs rasterize at `Height * sample`, then downscale): use `1` for effects that are only sharp edges (fill, outline with `Sharpness: 0`), `2` or `4` when blur, glow, bevel or soft outlines need smoother edges. It only affects raster quality, never on-screen size.

Python (`FontEffectScriptEmbedding`, available when `MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED` is on): `Mengine.createFontEffectFromFile(path, effectName, sample=1)`, `applyFontEffect(fontName, effect)` (font must not be compiled). Python has no entry point to build or inspect a `FontEffectDesc` from JSON: an effect is either loaded from a presets file (`createFontEffectFromFile`) or built in C++ (`createFontEffect`) and handed to a font.

## Presets file format

A presets file (the target of `FEPath`) is one JSON object with an `Effects` map at its root, parsed once per content and cached by the `fontEffect` dataflow. `Version` is written by tooling for humans; nothing in the engine reads or checks it.

```json
{
  "Version": 1,
  "Effects": {
    "<PresetName>": { "Sample": 2, "Layers": [ ... ] },
    "<AnotherPresetName>": { "Layers": [ ... ] }
  }
}
```

Each entry under `Effects` is a **font effect desc** (below) plus one extra, preset-only field:

| Field | Type | Default | Notes |
|---|---|---|---|
| `Sample` | integer | *(unset)* | Supersample hint for this preset. Used only if the font/`createFontEffectFromFile` call site did not pass an explicit `sample` (`FESample` in `Fonts.json` always wins over this hint). |

## Font effect desc

Every entry inside a presets file's `Effects` map, and every `FontEffectDesc` built directly in C++ via `createFontEffect`, has this shape:

```json
{ "Layers": [ <layer>, <layer>, ... ] }
```

| Field | Type | Required | Notes |
|---|---|---|---|
| `Layers` | array of **layer** | yes | 1 to 4 entries. Index 0 renders first (furthest back); later layers draw on top. A missing or empty `Layers` array fails at compile; a fifth entry is rejected by `addLayer()` when the file is replayed into the effect. |

### Layer

```json
{ "Enabled": true, "Opacity": 1.0, "Effects": [ <effect>, <effect>, ... ] }
```

| Field | Type | Default | Notes |
|---|---|---|---|
| `Enabled` | bool | `true` | A disabled layer is skipped entirely (no `TTFGlyph` quad is produced for it) but still counts toward the 4-layer limit. |
| `BlendMode` | string | `"Normal"` | How this layer composites onto the layout it merges into. Only meaningful together with `Merge`, since a layer that starts its own layout has nothing beneath it. |
| `Knockout` | bool | `false` | After the layer is composited, the glyph's own alpha is erased from the layout, which produces hollow text keeping only what falls outside the glyph — an outline, a shadow, a glow. |
| `Merge` | bool | `false` | Composites this layer into the previous layer's layout instead of starting a new one. The merged pair share one `TTFGlyph` quad, which is what makes a layer `BlendMode` expressible at all: separate layouts are blended by the render material, not by this rasterizer. The first enabled layer always starts a layout regardless of the flag. |
| `Opacity` | float | `1.0` | Multiplies the whole composited layer's alpha. Must be `>= 0`; no upper clamp. |
| `Effects` | array of **effect** | `[]` | Rendered in array order, each one composited `over` the previous within the same layer (`Blur` instead re-blurs everything composited so far in the layer). A layer with no enabled effects still produces one transparent-if-empty output; there is no fallback fill unless you put one in the array. |

### Effect (style)

One object per effect (`FontEffectStyleDesc` in C++ — "style" avoids the `FontEffectEffectDesc` stutter, the JSON key stays `Effects`); `Type` selects which of the other fields apply (fields for other types are ignored, not an error, so leftover fields from switching `Type` in an editor are harmless).

```json
{ "Type": "Outline", "Enabled": true, "Opacity": 1.0, "Color": [0, 0, 0, 255], "Width": 2.0, "Sharpness": 0.5 }
```

Common to every type:

| Field | Type | Default | Notes |
|---|---|---|---|
| `Type` | string | — | One of `Fill`, `Outline`, `Shadow`, `Glow`, `InnerShadow`, `InnerGlow`, `Bevel`, `Blur`, `Satin`, `Pattern`. Case-sensitive, exact match; an unrecognized value fails to load. |
| `Enabled` | bool | `true` | A disabled effect is skipped. |
| `Opacity` | float | `1.0` | Must be `>= 0`. |
| `BlendMode` | string | `"Normal"` | How this effect composites onto the effects before it in the same layer: `Normal`, `Multiply`, `Screen`, `Overlay`, `Darken`, `Lighten`, `Add`, `Subtract`, `Difference` or `Erase`. `Subtract` is what `FEPlugin` calls `op_blend_subtract`; `Erase` removes the effect's alpha from what is underneath, which is how a fill punches a hole through the effects below it. |

Per-type fields (all sizes/offsets/blur/spread are in font pixels at the effect's `Height`, independent of `sample`). Every `Color` field defaults to opaque white (`[255,255,255,255]`) when omitted:

| Type | Fields |
|---|---|
| `Fill` | `Color` (solid fill; ignored if `Gradient.Enabled` is true); `Gradient` — see below. |
| `Outline` | `Color`; `Width` (float, default `1.0`, `>= 0`); `Sharpness` (float, default `0.0`, `>= 0` — 0 is a hard edge, larger values soften the outline's own edge); `Position` (string, default `"Outside"` — `"Outside"`, `"Center"` or `"Inside"`, where the stroke sits relative to the glyph edge). |
| `Shadow`, `InnerShadow` | `Color`; `Offset` (`[x, y]`, default `[0, 0]`, `+y` is down); `Blur` (float, default `0.0`, `>= 0`); `Spread` (float, default `0.0`, `>= 0` — grows/shrinks the shape before blurring; `InnerShadow` still accepts it but the inner mask ignores growth beyond the glyph). |
| `Glow`, `InnerGlow` | `Color`; `Blur` (float, default `0.0`, `>= 0`); `Spread` (float, default `0.0`, `>= 0`). |
| `Bevel` | `Depth` (float, default `1.0`); `Size` (float, default `2.0`, `>= 0` — bevel ramp width); `Soften` (float, default `0.0`, `>= 0` — blurs the ramp's normal map); `Angle` (float degrees, default `120.0` — light direction); `Altitude` (float degrees, default `30.0` — light elevation); `Highlight` (color, default `[255,255,255,191]`); `Shadow` (color, default `[0,0,0,127]`). |
| `Blur` | `Blur` (float, default `0.0`, `>= 0` — re-blurs the layer's accumulated result so far; has no effect as the first effect in a layer, since there is nothing to blur yet). |
| `Pattern` | `Color` (tints the image; leave it white to show the image unchanged); `Pattern` — see below. |
| `Satin` | `Color`; `Distance` (float, default `0.0` — how far the two interfering copies of the glyph are displaced); `Angle` (float degrees, default `120.0` — displacement direction); `Blur` (float, default `0.0`, `>= 0`); `Invert` (bool, default `false`). The result is masked by the glyph, so satin only ever paints inside it. |

`Width`, `Sharpness`, `Blur`, `Spread`, `Size`, `Soften` must all be `>= 0`; a negative value fails validation.

### Gradient (`Fill` only)

```json
"Gradient": {
  "Enabled": true,
  "Angle": 90,
  "Space": "Glyph",
  "Stops": [
    { "T": 0.0, "Color": [255, 240, 160, 255] },
    { "T": 1.0, "Color": [200, 120, 0, 255] }
  ]
}
```

| Field | Type | Default | Notes |
|---|---|---|---|
| `Enabled` | bool | `false` | When `false` (or the whole `Gradient` object is absent), `Fill` uses its plain `Color` instead. |
| `Type` | string | `"Linear"` | `"Linear"`, `"Reflected"` (mirrored around the centre), `"Radial"`, `"Angle"` (sweep around the centre) or `"Distance"` (ramp follows the distance from the glyph edge inward — the shape burst `FEPlugin` calls `fill_radial`). |
| `Angle` | float degrees | `90.0` | Gradient direction; `90` runs top to bottom. Used by `Linear`, `Reflected` and `Angle`. |
| `Center` | `[x, y]` | `[0, 0]` | Shifts the gradient origin, in units of half the gradient space; `[0, 0]` is centred, `[-1, 0]` moves it to the left edge. |
| `Scale` | float | `1.0` | Stretches the gradient over a larger distance; smaller values concentrate the ramp. Must be `>= 0`. |
| `Reverse` | bool | `false` | Mirrors the stop positions. |
| `Dither` | bool | `false` | Adds one 8-bit least significant bit of ordered noise at the point of quantization, which removes banding on long ramps. |
| `Space` | string | `"Glyph"` | `"Glyph"` (gradient spans each glyph individually) or `"Font"` (gradient spans the whole rendered text run, glyphs sample their own slice of it). |
| `Stops` | array of `{ "T": float, "Color": color }` | `[]` | `T` in `0..1` along the gradient axis, ascending order (both the range and the order are validated). At least one stop is required when `Enabled` is `true`. |

Layers map one to one onto `TTFGlyph::quads`, and separate quads are blended by the render material rather than by this rasterizer. A layer `BlendMode` therefore only takes effect when the layer is merged into the previous one; `Merge` is what puts two layers in the same quad. `getLayoutCount()` counts enabled layers that start a layout, so merging reduces it.

`Position` changes what an outline paints. Before it existed, an outline filled everything within `Width` of the glyph edge, interior included, so the glyph body took the outline colour and whatever `Fill` followed had to paint over it. `"Outside"` now paints a true ring outside the edge. Every bundled preset draws an opaque `Fill` in the same layer, so they render identically; a preset whose fill is translucent, or which has no fill at all, will look different.

### Pattern (`Pattern` only)

| Field | Type | Default | Notes |
|---|---|---|---|
| `Path` | string | — | Image file, resolved against the presets file's own file group — that is, relative to that group's root, not to the directory the presets file sits in. Decoded once per effect at compile through `CODEC_SERVICE`, and two effects naming the same file share one decode. |
| `Codec` | string | *(inferred)* | Codec name; inferred from the extension when omitted. |
| `Tile` | string | `"Tile"` | `"Tile"` repeats, `"Mirror"` repeats alternating flipped, `"Clamp"` stretches the edge texels, `"Stretch"` fits one copy to the space. |
| `Scale` | float | `1.0` | Multiplies the image's native pixel size, so a pattern keeps its on-screen size whatever `Sample` is. Must be `>= 0`. |
| `Offset` | `[x, y]` | `[0, 0]` | Shifts the pattern, in font pixels. |
| `Angle` | float degrees | `0.0` | Rotates the pattern around the space origin. |
| `Space` | string | `"Glyph"` | `"Glyph"` anchors the pattern per glyph, `"Font"` anchors it to the text run so it stays continuous across glyphs. |

The image is decoded synchronously inside `_compile()`, which already runs off the render-critical path when a font compiles. A two phase prefetch for `FontEffectFile` — prefetch the desc, parse it, then prefetch the images it names — is a follow-up, not implemented.

### Color

Every color field (`Color`, `Highlight`, `Shadow`, gradient stop `Color`) is read with the engine's standard `Helper::getJSONColor`, so it accepts exactly what colors accept everywhere else in Mengine: `[r, g, b, a]` as integers `0..255` or as floats `0..1`, an object `{ "r": , "g": , "b": , "a": }`, or a hex string `"#RRGGBB"` / `"#RRGGBBAA"` (alpha defaults to `FF`, case-insensitive). Omit the field entirely to keep the default.

Values are converted to premultiplied-alpha internally; the JSON itself is always straight (non-premultiplied) RGBA. The plugin itself only ever reads a desc (it has no dump/export API at all); `examples/font_effect_viewer` is the only place that serializes a desc back to JSON (Save), and it always writes colors as `[r, g, b, a]` (0-255 integers).

### Offset / vector fields

`Offset` is read with `Helper::getJSONVec2f`: a 2-element array `[x, y]` or an object `{ "x": , "y": }`, in font pixels; `+x` is right, `+y` is down. Offsets are sampled at subpixel precision, so a fractional value moves the shape smoothly rather than snapping to whole pixels.

`Blur` is a true Gaussian: a separable kernel while its radius stays small, and four fractional box passes beyond that, where the box width varies continuously with the radius so a growing blur does not step. Distances are measured from a distance field seeded with the rasterizer's own subpixel coverage rather than from a thresholded mask, which is what keeps outline, spread, bevel and the `Distance` gradient smooth along an edge.

## Worked example

```json
{
  "Version": 1,
  "Effects": {
    "GoldTitle": {
      "Sample": 2,
      "Layers": [
        {
          "Enabled": true,
          "Opacity": 1.0,
          "Effects": [
            { "Type": "Shadow", "Color": [0, 0, 0, 160], "Offset": [2, 3], "Blur": 3.0, "Spread": 0.0 },
            { "Type": "Outline", "Color": [40, 20, 0, 255], "Width": 2.0, "Sharpness": 0.5 },
            { "Type": "Fill", "Color": [255, 220, 80, 255] }
          ]
        },
        {
          "Effects": [
            {
              "Type": "Fill",
              "Gradient": {
                "Enabled": true,
                "Angle": 90,
                "Space": "Glyph",
                "Stops": [
                  { "T": 0.0, "Color": [255, 240, 160, 255] },
                  { "T": 1.0, "Color": [200, 120, 0, 255] }
                ]
              }
            },
            { "Type": "InnerShadow", "Color": [0, 0, 0, 120], "Offset": [0, 2], "Blur": 2.0 },
            {
              "Type": "Bevel",
              "Depth": 2.0, "Size": 3.0, "Soften": 1.0, "Angle": 120, "Altitude": 30,
              "Highlight": [255, 255, 255, 200], "Shadow": [0, 0, 0, 160]
            },
            { "Type": "Glow", "Enabled": false, "Color": [255, 200, 0, 255], "Blur": 4.0, "Spread": 1.0, "Opacity": 0.6 }
          ]
        }
      ]
    },
    "HudOutline": {
      "Layers": [
        { "Effects": [ { "Type": "Outline", "Color": [0, 0, 0, 255], "Width": 1.5 }, { "Type": "Fill", "Color": "#FFFFFF" } ] }
      ]
    }
  }
}
```

`GoldTitle` needs two `TTFGlyph::quads` (one per layer); `HudOutline` needs one. A font requesting a preset that needs more than 4 layers, or with an invalid field, fails to compile — the rejecting setter logs exactly which one, e.g. `font effect max layers 4` or `font effect invalid effect blur -1.000000`.

## Editor

`examples/font_effect_viewer` is an ImGui-based viewer/editor for this format: load and save presets files, edit every field above with live preview, pick a glyph/TTF and text, and zoom the preview canvas. It writes exactly the schema documented here.
