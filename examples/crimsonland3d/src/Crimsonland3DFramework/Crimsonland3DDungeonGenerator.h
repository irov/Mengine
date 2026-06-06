#pragma once

#include "Kernel/VectorRenderVertex3D.h"
#include "Kernel/VectorRenderIndex32.h"
#include "Kernel/Vector.h"

#include "Config/StdInt.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // Parameters controlling procedural dungeon generation.
    // The dungeon is built on a square grid of walkable/solid cells, then
    // extruded into a top-down "cross-section" mesh (floor tiles + wall blocks).
    //////////////////////////////////////////////////////////////////////////
    struct Crimsonland3DDungeonParams
    {
        uint32_t gridWidth = 48;
        uint32_t gridHeight = 48;
        float cellSize = 120.f;
        float wallHeight = 150.f;
        uint32_t roomCount = 12;
        uint32_t roomMinSize = 4;
        uint32_t roomMaxSize = 9;
        uint32_t corridorRadius = 1;
        uint32_t seed = 1u;

        // Rocky "cliff" detail: how many sub-quads each wall edge is split into,
        // and how strongly interior vertices are pushed for a cave-rock look.
        uint32_t wallSegments = 3;
        uint32_t wallHeightSegments = 2;
        float wallRoughness = 0.22f;   // fraction of cellSize (horizontal bump)
        float topRoughness = 0.16f;    // fraction of wallHeight (corner height jitter)
        float wallEdgeWander = 0.20f;  // fraction of cellSize (per-corner horizontal drift)
        float wallSkirt = 0.30f;       // fraction of cellSize (depth the wall tucks below the floor)

        // Multi-level terrain: rooms sit on discrete elevations and corridors ramp
        // smoothly between them so the player can walk up onto plateaus or down
        // into pits.
        uint32_t elevationLevels = 3;  // number of distinct height tiers (>=1)
        float elevationStep = 70.f;    // world height between adjacent tiers
    };
    //////////////////////////////////////////////////////////////////////////
    // Result of generation: two separate low-poly meshes (floor and walls) so
    // they can use distinct materials/colors, plus the world-space spawn point
    // located at the center of the first carved room. The occupancy grid is also
    // returned so gameplay code can query walkability (camera framing, collision).
    //////////////////////////////////////////////////////////////////////////
    struct Crimsonland3DDungeonBuildResult
    {
        VectorRenderVertex3D floorVertices;
        VectorRenderIndex32 floorIndices;

        VectorRenderVertex3D wallVertices;
        VectorRenderIndex32 wallIndices;

        mt::vec3f spawnPosition = {0.f, 0.f, 0.f};

        Vector<uint8_t> cells;          // 0 = solid rock, 1 = walkable floor
        Vector<float> floorElevation;   // per-cell floor height (world units); only valid for walkable cells
        uint32_t gridWidth = 0;
        uint32_t gridHeight = 0;
        float cellSize = 0.f;
        float wallHeight = 0.f;
        float originX = 0.f;
        float originZ = 0.f;

        // Returns true if the world-space point (x,z) lies on a walkable floor cell.
        bool isWalkableWorld( float _x, float _z ) const;

        // Bilinearly samples the walkable floor height (world units) at (x,z).
        // Falls back to 0 outside the grid.
        float floorHeightWorld( float _x, float _z ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    class Crimsonland3DDungeonGenerator
    {
    public:
        static Crimsonland3DDungeonBuildResult generate( const Crimsonland3DDungeonParams & _params );
    };
    //////////////////////////////////////////////////////////////////////////
}
