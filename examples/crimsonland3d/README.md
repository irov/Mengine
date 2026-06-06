# Crimsonland3D — Mengine 3D pipeline showcase

Minimal example wiring up the new 3D upgrade pipeline (phases C1..C7):

| Phase | Component                                | Used here                                |
|-------|------------------------------------------|------------------------------------------|
| C1-C3 | DX9 Vertex3D / unlit Mesh3D / shaders    | Material_OpaqueUnlit / Material_OpaqueLit |
| C4    | SkinnedMesh3D + bone palette upload      | (TODO: drop a glTF rig)                  |
| C5    | GLTFImporterPlugin (cgltf-based)         | enabled in Configs.json                  |
| C6    | Lighting3D + DirectionalLight3D / PointLight3D / BlobShadow3D | created in scene |
| C7    | PostProcessFx3D (RT + tonemap + vignette + chromatic + hit-flash) | scene root |

## Build (Win32, MSVC 2022)

```
build\msvc17\make_solution_debug.bat
```

Open the generated `solutions/solution_msvc17/Crimsonland3D_WIN32.sln`, set
`Win32Application` as startup, run.

## Live key handlers

| Key   | Action                                         |
|-------|------------------------------------------------|
| 1 / 2 | Decrease / increase exposure (×0.9 / ×1.1)     |
| 3 / 4 | Weaker / stronger vignette                     |
| 5 / 6 | Chromatic offset −0.5 / +0.5 px                |
| Space | Punch hit-flash overlay (red, intensity 0.6)   |

## Adding a real 3D mesh

The example currently creates only lights + blob shadow + post-fx. To see the
lighting actually bounce off geometry, drop a `.gltf` asset into
`resources/.downloads/assets/` and uncomment the Mesh3D block in
[`Crimsonland3DSceneEventReceiver.cpp`](src/Crimsonland3DFramework/Crimsonland3DSceneEventReceiver.cpp).
The `Material_OpaqueLit` / `Material_SkinnedOpaqueLit` materials are
already declared in `Standart.xml`.
