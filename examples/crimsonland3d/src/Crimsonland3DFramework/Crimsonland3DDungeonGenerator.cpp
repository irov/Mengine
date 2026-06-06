#include "Crimsonland3DDungeonGenerator.h"

#include "Kernel/Vector.h"

#include "Config/StdMath.h"

#include <utility>

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        // Deterministic linear congruential generator so the same seed always
        // produces the same dungeon layout.
        //////////////////////////////////////////////////////////////////////////
        struct DungeonRandom
        {
            uint32_t state;

            explicit DungeonRandom( uint32_t _seed )
                : state( _seed != 0u ? _seed : 0x1u )
            {
            }

            uint32_t next()
            {
                state = state * 1664525u + 1013904223u;

                return state;
            }

            // Returns a value in [_min, _max] inclusive.
            uint32_t range( uint32_t _min, uint32_t _max )
            {
                if( _max <= _min )
                {
                    return _min;
                }

                uint32_t span = _max - _min + 1u;

                return _min + (this->next() >> 8) % span;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        enum DungeonCell : uint8_t
        {
            DC_SOLID = 0,
            DC_FLOOR = 1,
        };
        //////////////////////////////////////////////////////////////////////////
        struct DungeonRoom
        {
            uint32_t x;
            uint32_t z;
            uint32_t w;
            uint32_t h;
            uint32_t colorVariant;
            int32_t level;
        };
        //////////////////////////////////////////////////////////////////////////
        static uint32_t packRGBA8( float _r, float _g, float _b, float _a )
        {
            auto pack = []( float _v ) -> uint32_t
            {
                if( _v < 0.f ) _v = 0.f;
                if( _v > 1.f ) _v = 1.f;

                return (uint32_t)(_v * 255.f + 0.5f);
            };

            return pack( _r ) | (pack( _g ) << 8) | (pack( _b ) << 16) | (pack( _a ) << 24);
        }
        //////////////////////////////////////////////////////////////////////////
        static void appendQuad( VectorRenderVertex3D * const _vertices, VectorRenderIndex32 * const _indices
            , const mt::vec3f & _p0, const mt::vec3f & _p1, const mt::vec3f & _p2, const mt::vec3f & _p3
            , const mt::vec3f & _normal, uint32_t _color )
        {
            const VectorRenderVertex3D::size_type base = _vertices->size();

            RenderVertex3D v;
            v.normal = _normal;
            v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
            v.color = _color;
            v._padding = 0;

            v.position = _p0;
            v.uv[0] = mt::vec2f( 0.f, 0.f );
            v.uv[1] = v.uv[0];
            _vertices->push_back( v );

            v.position = _p1;
            v.uv[0] = mt::vec2f( 1.f, 0.f );
            v.uv[1] = v.uv[0];
            _vertices->push_back( v );

            v.position = _p2;
            v.uv[0] = mt::vec2f( 1.f, 1.f );
            v.uv[1] = v.uv[0];
            _vertices->push_back( v );

            v.position = _p3;
            v.uv[0] = mt::vec2f( 0.f, 1.f );
            v.uv[1] = v.uv[0];
            _vertices->push_back( v );

            _indices->push_back( (RenderIndex32)(base + 0) );
            _indices->push_back( (RenderIndex32)(base + 1) );
            _indices->push_back( (RenderIndex32)(base + 2) );
            _indices->push_back( (RenderIndex32)(base + 0) );
            _indices->push_back( (RenderIndex32)(base + 2) );
            _indices->push_back( (RenderIndex32)(base + 3) );
        }
        //////////////////////////////////////////////////////////////////////////
        // Carve an axis-aligned rectangle of floor cells into the grid.
        //////////////////////////////////////////////////////////////////////////
        static void carveRect( Vector<uint8_t> & _grid, uint32_t _gridWidth, uint32_t _gridHeight
            , uint32_t _x0, uint32_t _z0, uint32_t _x1, uint32_t _z1 )
        {
            for( uint32_t z = _z0; z <= _z1 && z < _gridHeight; ++z )
            {
                for( uint32_t x = _x0; x <= _x1 && x < _gridWidth; ++x )
                {
                    _grid[z * _gridWidth + x] = DC_FLOOR;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // Carve a horizontal corridor between two grid columns at a given row,
        // thickened by the corridor radius.
        //////////////////////////////////////////////////////////////////////////
        static void carveHorizontal( Vector<uint8_t> & _grid, uint32_t _gridWidth, uint32_t _gridHeight
            , uint32_t _xa, uint32_t _xb, uint32_t _z, uint32_t _radius )
        {
            uint32_t xmin = _xa < _xb ? _xa : _xb;
            uint32_t xmax = _xa < _xb ? _xb : _xa;

            uint32_t z0 = _z > _radius ? _z - _radius : 0u;
            uint32_t z1 = _z + _radius;

            carveRect( _grid, _gridWidth, _gridHeight, xmin, z0, xmax, z1 );
        }
        //////////////////////////////////////////////////////////////////////////
        // Carve a vertical corridor between two grid rows at a given column,
        // thickened by the corridor radius.
        //////////////////////////////////////////////////////////////////////////
        static void carveVertical( Vector<uint8_t> & _grid, uint32_t _gridWidth, uint32_t _gridHeight
            , uint32_t _za, uint32_t _zb, uint32_t _x, uint32_t _radius )
        {
            uint32_t zmin = _za < _zb ? _za : _zb;
            uint32_t zmax = _za < _zb ? _zb : _za;

            uint32_t x0 = _x > _radius ? _x - _radius : 0u;
            uint32_t x1 = _x + _radius;

            carveRect( _grid, _gridWidth, _gridHeight, x0, zmin, x1, zmax );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isFloor( const Vector<uint8_t> & _grid, uint32_t _gridWidth, uint32_t _gridHeight, int32_t _x, int32_t _z )
        {
            if( _x < 0 || _z < 0 || _x >= (int32_t)_gridWidth || _z >= (int32_t)_gridHeight )
            {
                return false;
            }

            return _grid[(uint32_t)_z * _gridWidth + (uint32_t)_x] == DC_FLOOR;
        }
        //////////////////////////////////////////////////////////////////////////
        // Stateless hash -> [0,1). Used so that wall "pieces" sharing a grid corner
        // always agree on its perturbation, which is what makes the LEGO/cliff
        // tiles glue together seamlessly (Warcraft III style).
        //////////////////////////////////////////////////////////////////////////
        static float hash01( uint32_t _a, uint32_t _b, uint32_t _seed )
        {
            uint32_t h = _a * 374761393u + _b * 668265263u + _seed * 2246822519u;
            h = (h ^ (h >> 13)) * 1274126177u;
            h ^= h >> 16;

            return (float)(h & 0x00FFFFFFu) / (float)0x01000000u;
        }
        //////////////////////////////////////////////////////////////////////////
        // Deterministic top height for a grid corner: every wall/cap that touches
        // this corner reads the same value, so edges match exactly.
        //////////////////////////////////////////////////////////////////////////
        static float cornerHeight( uint32_t _gx, uint32_t _gz, float _wallHeight, float _jitter, uint32_t _seed )
        {
            float n = hash01( _gx, _gz, _seed ^ 0xC0FFEEu );

            return _wallHeight + (n * 2.f - 1.f) * _jitter;
        }
        //////////////////////////////////////////////////////////////////////////
        // Deterministic horizontal wander for a grid corner. Every wall face and
        // the top cap touching this corner read the same offset, so the whole rock
        // silhouette becomes organic/cave-like while staying perfectly welded
        // (no holes at convex corners).
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f cornerOffset( uint32_t _gx, uint32_t _gz, float _wander, uint32_t _seed )
        {
            float a = hash01( _gx, _gz, _seed ^ 0x1357Bu );
            float b = hash01( _gx, _gz, _seed ^ 0x9E3779u );

            return mt::vec2f( (a * 2.f - 1.f) * _wander, (b * 2.f - 1.f) * _wander );
        }
        //////////////////////////////////////////////////////////////////////////
        // Smooth bump shape that is 0 at the edges (u = 0 / u = 1) and peaks in the
        // middle. Keeping the edges flat is the trick that makes neighbouring wall
        // pieces stay welded while the interior gets a randomised rocky surface.
        //////////////////////////////////////////////////////////////////////////
        static float edgeBump( float _u )
        {
            return 4.f * _u * (1.f - _u);
        }
        //////////////////////////////////////////////////////////////////////////
        // Build one cave-rock wall piece: a vertical strip from floor (y = 0) up to
        // the two corner heights. The shared corner columns (i = 0/N) are displaced
        // by the deterministic per-corner wander so adjacent faces and the cap stay
        // welded; interior columns add extra organic noise and bulge into the room
        // toward mid-height, giving a rounded cavern wall instead of a flat slab.
        //////////////////////////////////////////////////////////////////////////
        static void appendRockWall( VectorRenderVertex3D * const _vertices, VectorRenderIndex32 * const _indices
            , const mt::vec2f & _a, const mt::vec2f & _b
            , const mt::vec2f & _offsetA, const mt::vec2f & _offsetB
            , float _topA, float _topB
            , float _baseA, float _baseB
            , const mt::vec3f & _normal, uint32_t _color
            , uint32_t _widthSegments, uint32_t _heightSegments, float _amplitude, float _skirt, uint32_t _hashSeed )
        {
            const uint32_t N = _widthSegments < 1u ? 1u : _widthSegments;
            const uint32_t M = _heightSegments < 1u ? 1u : _heightSegments;

            const VectorRenderVertex3D::size_type base = _vertices->size();

            const float tangentX = -_normal.z;
            const float tangentZ = _normal.x;

            for( uint32_t i = 0; i <= N; ++i )
            {
                float u = (float)i / (float)N;

                float hx = _a.x + (_b.x - _a.x) * u;
                float hz = _a.y + (_b.y - _a.y) * u;

                // Shared corner wander, linearly blended across the face. Endpoints
                // equal the exact corner offsets used by every neighbour and the cap.
                float cornerOX = _offsetA.x + (_offsetB.x - _offsetA.x) * u;
                float cornerOZ = _offsetA.y + (_offsetB.y - _offsetA.y) * u;

                float topY = _topA + (_topB - _topA) * u;

                float baseY = _baseA + (_baseB - _baseA) * u;

                float bump = edgeBump( u );

                for( uint32_t j = 0; j <= M; ++j )
                {
                    float t = (float)j / (float)M;          // 0 at top, 1 at floor
                    float vfac = 1.f - t;                   // height blend (1 top -> 0 floor)

                    // Weight that is 0 at top (meets cap) and floor (meets ground),
                    // peaks in the middle -> no torn seams.
                    float vWeight = 4.f * t * (1.f - t);

                    // The corner wander is faded out toward the bottom so the wall
                    // base stays exactly on the grid line (welded to the floor edge),
                    // while the top keeps the organic cave silhouette that matches
                    // the cap. Adjacent faces share this identical profile -> welded.
                    float baseOX = cornerOX * vfac;
                    float baseOZ = cornerOZ * vfac;

                    float disp = 0.f;
                    float slide = 0.f;
                    float bulge = 0.f;

                    if( i != 0u && i != N )
                    {
                        // Two octaves of stateless noise -> chiselled rock face.
                        float dn = hash01( _hashSeed + i, j, 0xBEEFu );
                        float dn2 = hash01( _hashSeed + i * 7u + 3u, j * 13u + 1u, 0xA53Fu );

                        float n = (dn * 2.f - 1.f) * 0.6f + (dn2 * 2.f - 1.f) * 0.4f;

                        disp = _amplitude * n * bump * vWeight;

                        float sn = hash01( _hashSeed + i * 5u + 11u, j * 3u + 7u, 0x77A1u );
                        slide = _amplitude * 0.7f * (sn * 2.f - 1.f) * bump * vWeight;

                        // Cavern bulge: the interior of the face leans into the room
                        // around mid height. Kept off the edge columns so convex
                        // corners (different normals) never split open.
                        bulge = _amplitude * 0.9f * (4.f * t * (1.f - t)) * bump;
                    }

                    float offX = baseOX + _normal.x * (disp + bulge) + tangentX * slide;
                    float offZ = baseOZ + _normal.z * (disp + bulge) + tangentZ * slide;

                    // Extend the base below the floor (skirt) so the wall always tucks
                    // under the floor tiles and never reveals a gap at the seam. The
                    // bottom tracks the local floor elevation so walls stay welded to
                    // terrain that steps up or down.
                    float y = topY * vfac + (baseY - _skirt) * t;

                    RenderVertex3D v;
                    v.position = mt::vec3f( hx + offX, y, hz + offZ );
                    v.normal = _normal;
                    v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
                    v.color = _color;
                    v.uv[0] = mt::vec2f( u, 1.f - vfac );
                    v.uv[1] = v.uv[0];
                    v._padding = 0;

                    _vertices->push_back( v );
                }
            }

            const uint32_t stride = M + 1u;

            for( uint32_t i = 0; i < N; ++i )
            {
                for( uint32_t j = 0; j < M; ++j )
                {
                    uint32_t v00 = (uint32_t)base + (i + 0u) * stride + (j + 0u);
                    uint32_t v10 = (uint32_t)base + (i + 1u) * stride + (j + 0u);
                    uint32_t v11 = (uint32_t)base + (i + 1u) * stride + (j + 1u);
                    uint32_t v01 = (uint32_t)base + (i + 0u) * stride + (j + 1u);

                    _indices->push_back( (RenderIndex32)v00 );
                    _indices->push_back( (RenderIndex32)v10 );
                    _indices->push_back( (RenderIndex32)v11 );
                    _indices->push_back( (RenderIndex32)v00 );
                    _indices->push_back( (RenderIndex32)v11 );
                    _indices->push_back( (RenderIndex32)v01 );
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // Scatter prop: a low-poly boulder. Built as a deformed octahedron (6
        // base vertices) whose corners are pushed by stateless noise, then placed
        // sitting on the floor at (_cx, _cz). Cheap and reads as a cave rock.
        //////////////////////////////////////////////////////////////////////////
        static void appendBoulder( VectorRenderVertex3D * const _vertices, VectorRenderIndex32 * const _indices
            , float _cx, float _cz, float _baseY, float _radius, uint32_t _color, uint32_t _seed )
        {
            // Small rounded stone: a low-poly UV sphere flattened into a pebble and
            // softly perturbed so it reads as a natural rock rather than a spike.
            const uint32_t stacks = 3u;   // vertical rings
            const uint32_t slices = 7u;   // around

            const float flatten = 0.62f;  // squash vertically -> pebble shape
            const float domeH = _radius * 2.f * flatten;

            // Build a vertex grid (stacks+1) x (slices+1).
            Vector<mt::vec3f> pts;
            pts.resize( (VectorRenderVertex3D::size_type)(stacks + 1u) * (slices + 1u) );

            for( uint32_t s = 0; s <= stacks; ++s )
            {
                float phi = StdMath::constant_pi * ((float)s / (float)stacks);   // 0..pi
                float sy = StdMath::cosf( phi );                                 // 1..-1
                float sr = StdMath::sinf( phi );

                for( uint32_t a = 0; a <= slices; ++a )
                {
                    float theta = StdMath::constant_two_pi * ((float)a / (float)slices);

                    // Mild per-vertex radius noise keeps the surface organic but round.
                    float n = 0.85f + 0.18f * hash01( _seed + s * 17u + 1u, a * 13u + 5u, 0x2B1Du );

                    float r = _radius * n;

                    float px = _cx + StdMath::cosf( theta ) * sr * r;
                    float pz = _cz + StdMath::sinf( theta ) * sr * r;
                    float py = _baseY + (sy * 0.5f + 0.5f) * domeH;             // rest on the floor

                    pts[s * (slices + 1u) + a] = mt::vec3f( px, py, pz );
                }
            }

            // Triangulate the grid.
            for( uint32_t s = 0; s < stacks; ++s )
            {
                for( uint32_t a = 0; a < slices; ++a )
                {
                    const mt::vec3f & p00 = pts[s * (slices + 1u) + a];
                    const mt::vec3f & p10 = pts[(s + 1u) * (slices + 1u) + a];
                    const mt::vec3f & p11 = pts[(s + 1u) * (slices + 1u) + a + 1u];
                    const mt::vec3f & p01 = pts[s * (slices + 1u) + a + 1u];

                    const mt::vec3f quad[4] = { p00, p01, p11, p10 };
                    const uint32_t tris[2][3] = { {0, 1, 2}, {0, 2, 3} };

                    for( uint32_t ti = 0; ti != 2u; ++ti )
                    {
                        const mt::vec3f & a0 = quad[tris[ti][0]];
                        const mt::vec3f & b0 = quad[tris[ti][1]];
                        const mt::vec3f & c0 = quad[tris[ti][2]];

                        mt::vec3f e0 = b0 - a0;
                        mt::vec3f e1 = c0 - a0;

                        mt::vec3f nrm;
                        mt::cross_v3_v3( &nrm, e0, e1 );
                        mt::norm_safe_v3( &nrm, nrm );

                        const VectorRenderVertex3D::size_type fb = _vertices->size();

                        const mt::vec3f tri[3] = { a0, b0, c0 };

                        for( uint32_t vi = 0; vi != 3u; ++vi )
                        {
                            RenderVertex3D v;
                            v.position = tri[vi];
                            v.normal = nrm;
                            v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
                            v.color = _color;
                            v.uv[0] = mt::vec2f( 0.f, 0.f );
                            v.uv[1] = v.uv[0];
                            v._padding = 0;

                            _vertices->push_back( v );
                        }

                        _indices->push_back( (RenderIndex32)(fb + 0) );
                        _indices->push_back( (RenderIndex32)(fb + 1) );
                        _indices->push_back( (RenderIndex32)(fb + 2) );
                    }
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
        // Double-sided vertical quad (visible from both faces). Used for the outer
        // rim where we don't want to reason about culling direction.
        //////////////////////////////////////////////////////////////////////////
        static void appendQuadDoubleSided( VectorRenderVertex3D * const _vertices, VectorRenderIndex32 * const _indices
            , const mt::vec3f & _p0, const mt::vec3f & _p1, const mt::vec3f & _p2, const mt::vec3f & _p3
            , const mt::vec3f & _normal, uint32_t _color )
        {
            appendQuad( _vertices, _indices, _p0, _p1, _p2, _p3, _normal, _color );
            appendQuad( _vertices, _indices, _p3, _p2, _p1, _p0, -_normal, _color );
        }
        //////////////////////////////////////////////////////////////////////////
        // A solid rock slab (box) used to wall off the world border. Emits a top
        // cap plus double-sided vertical faces so the camera never sees through it.
        // The top is jittered per corner for a rough mountain-rim silhouette.
        //////////////////////////////////////////////////////////////////////////
        static void appendBorderSlab( VectorRenderVertex3D * const _vertices, VectorRenderIndex32 * const _indices
            , float _x0, float _x1, float _z0, float _z1
            , float _yTop, float _yBottom, float _jitter, uint32_t _topColor, uint32_t _sideColor, uint32_t _seed )
        {
            float t00 = _yTop + (hash01( (uint32_t)(_x0 * 0.013f) + 1u, (uint32_t)(_z0 * 0.013f) + 1u, _seed ) * 2.f - 1.f) * _jitter;
            float t10 = _yTop + (hash01( (uint32_t)(_x1 * 0.013f) + 5u, (uint32_t)(_z0 * 0.013f) + 1u, _seed ) * 2.f - 1.f) * _jitter;
            float t11 = _yTop + (hash01( (uint32_t)(_x1 * 0.013f) + 5u, (uint32_t)(_z1 * 0.013f) + 9u, _seed ) * 2.f - 1.f) * _jitter;
            float t01 = _yTop + (hash01( (uint32_t)(_x0 * 0.013f) + 1u, (uint32_t)(_z1 * 0.013f) + 9u, _seed ) * 2.f - 1.f) * _jitter;

            // Top cap.
            appendQuad( _vertices, _indices
                , mt::vec3f( _x0, t00, _z0 )
                , mt::vec3f( _x1, t10, _z0 )
                , mt::vec3f( _x1, t11, _z1 )
                , mt::vec3f( _x0, t01, _z1 )
                , mt::vec3f( 0.f, 1.f, 0.f ), _topColor );

            // Four vertical sides (double sided).
            appendQuadDoubleSided( _vertices, _indices
                , mt::vec3f( _x0, t00, _z0 ), mt::vec3f( _x1, t10, _z0 )
                , mt::vec3f( _x1, _yBottom, _z0 ), mt::vec3f( _x0, _yBottom, _z0 )
                , mt::vec3f( 0.f, 0.f, -1.f ), _sideColor );

            appendQuadDoubleSided( _vertices, _indices
                , mt::vec3f( _x1, t11, _z1 ), mt::vec3f( _x0, t01, _z1 )
                , mt::vec3f( _x0, _yBottom, _z1 ), mt::vec3f( _x1, _yBottom, _z1 )
                , mt::vec3f( 0.f, 0.f, 1.f ), _sideColor );

            appendQuadDoubleSided( _vertices, _indices
                , mt::vec3f( _x0, t01, _z1 ), mt::vec3f( _x0, t00, _z0 )
                , mt::vec3f( _x0, _yBottom, _z0 ), mt::vec3f( _x0, _yBottom, _z1 )
                , mt::vec3f( -1.f, 0.f, 0.f ), _sideColor );

            appendQuadDoubleSided( _vertices, _indices
                , mt::vec3f( _x1, t10, _z0 ), mt::vec3f( _x1, t11, _z1 )
                , mt::vec3f( _x1, _yBottom, _z1 ), mt::vec3f( _x1, _yBottom, _z0 )
                , mt::vec3f( 1.f, 0.f, 0.f ), _sideColor );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DDungeonBuildResult Crimsonland3DDungeonGenerator::generate( const Crimsonland3DDungeonParams & _params )
    {
        Crimsonland3DDungeonBuildResult result;

        const uint32_t gridWidth = _params.gridWidth;
        const uint32_t gridHeight = _params.gridHeight;

        if( gridWidth < 4u || gridHeight < 4u )
        {
            return result;
        }

        Detail::DungeonRandom rng( _params.seed );

        Vector<uint8_t> grid;
        grid.resize( (Vector<uint8_t>::size_type)gridWidth * gridHeight, Detail::DC_SOLID );

        // 1) Place non-overlapping rectangular rooms (leave a 1-cell solid border).
        Vector<Detail::DungeonRoom> rooms;

        const uint32_t maxAttempts = _params.roomCount * 12u;

        for( uint32_t attempt = 0; attempt != maxAttempts && rooms.size() < _params.roomCount; ++attempt )
        {
            uint32_t w = rng.range( _params.roomMinSize, _params.roomMaxSize );
            uint32_t h = rng.range( _params.roomMinSize, _params.roomMaxSize );

            if( w + 2u >= gridWidth || h + 2u >= gridHeight )
            {
                continue;
            }

            uint32_t x = rng.range( 1u, gridWidth - w - 2u );
            uint32_t z = rng.range( 1u, gridHeight - h - 2u );

            // Reject rooms that overlap an existing room (with a 1-cell gap).
            bool overlap = false;

            for( const Detail::DungeonRoom & room : rooms )
            {
                if( x <= room.x + room.w + 1u && x + w + 1u >= room.x &&
                    z <= room.z + room.h + 1u && z + h + 1u >= room.z )
                {
                    overlap = true;

                    break;
                }
            }

            if( overlap == true )
            {
                continue;
            }

            Detail::DungeonRoom room;
            room.x = x;
            room.z = z;
            room.w = w;
            room.h = h;
            room.colorVariant = rng.range( 0u, 3u );
            room.level = 0;

            rooms.push_back( room );

            Detail::carveRect( grid, gridWidth, gridHeight, x, z, x + w - 1u, z + h - 1u );
        }

        if( rooms.empty() == true )
        {
            return result;
        }

        // 2) Connect rooms sequentially with L-shaped corridors (tunnels).
        for( Vector<Detail::DungeonRoom>::size_type i = 1; i < rooms.size(); ++i )
        {
            const Detail::DungeonRoom & a = rooms[i - 1];
            const Detail::DungeonRoom & b = rooms[i];

            uint32_t ax = a.x + a.w / 2u;
            uint32_t az = a.z + a.h / 2u;
            uint32_t bx = b.x + b.w / 2u;
            uint32_t bz = b.z + b.h / 2u;

            // Randomize the elbow order so corridors do not all bend the same way.
            if( (rng.next() & 1u) == 0u )
            {
                Detail::carveHorizontal( grid, gridWidth, gridHeight, ax, bx, az, _params.corridorRadius );
                Detail::carveVertical( grid, gridWidth, gridHeight, az, bz, bx, _params.corridorRadius );
            }
            else
            {
                Detail::carveVertical( grid, gridWidth, gridHeight, az, bz, ax, _params.corridorRadius );
                Detail::carveHorizontal( grid, gridWidth, gridHeight, ax, bx, bz, _params.corridorRadius );
            }
        }

        // 2b) Assign each room a discrete elevation tier (the first/spawn room
        //     stays at level 0). Tiers spread symmetrically around 0 so we get
        //     both raised plateaus and sunken pits.
        const uint32_t elevTiers = _params.elevationLevels < 1u ? 1u : _params.elevationLevels;
        const int32_t elevSpan = (int32_t)(elevTiers - 1u);

        for( Vector<Detail::DungeonRoom>::size_type i = 1; i < rooms.size(); ++i )
        {
            int32_t lvl = (int32_t)rng.range( 0u, (uint32_t)elevSpan ) - elevSpan / 2;
            rooms[i].level = lvl;
        }

        // 2c) Build a smooth per-cell elevation field. Room interiors are clamped
        //     to their tier (Dirichlet boundary); corridor cells are solved by
        //     Jacobi relaxation so they become gentle walkable ramps between rooms.
        Vector<float> elev;
        elev.resize( grid.size(), 0.f );

        Vector<uint8_t> elevFixed;
        elevFixed.resize( grid.size(), 0u );

        for( const Detail::DungeonRoom & room : rooms )
        {
            float h = (float)room.level * _params.elevationStep;

            for( uint32_t z = room.z; z < room.z + room.h && z < gridHeight; ++z )
            {
                for( uint32_t x = room.x; x < room.x + room.w && x < gridWidth; ++x )
                {
                    elev[z * gridWidth + x] = h;
                    elevFixed[z * gridWidth + x] = 1u;
                }
            }
        }

        {
            Vector<float> elevNext = elev;

            for( uint32_t iter = 0; iter != 60u; ++iter )
            {
                for( uint32_t z = 0; z != gridHeight; ++z )
                {
                    for( uint32_t x = 0; x != gridWidth; ++x )
                    {
                        uint32_t idx = z * gridWidth + x;

                        if( grid[idx] != Detail::DC_FLOOR || elevFixed[idx] != 0u )
                        {
                            continue;
                        }

                        float sum = 0.f;
                        float count = 0.f;

                        const int32_t dx[4] = { -1, 1, 0, 0 };
                        const int32_t dz[4] = { 0, 0, -1, 1 };

                        for( uint32_t k = 0; k != 4u; ++k )
                        {
                            int32_t nx = (int32_t)x + dx[k];
                            int32_t nz = (int32_t)z + dz[k];

                            if( nx < 0 || nz < 0 || nx >= (int32_t)gridWidth || nz >= (int32_t)gridHeight )
                            {
                                continue;
                            }

                            uint32_t nidx = (uint32_t)nz * gridWidth + (uint32_t)nx;

                            if( grid[nidx] != Detail::DC_FLOOR )
                            {
                                continue;
                            }

                            sum += elev[nidx];
                            count += 1.f;
                        }

                        if( count > 0.f )
                        {
                            elevNext[idx] = sum / count;
                        }
                    }
                }

                elev.swap( elevNext );
            }
        }

        // 3) Build geometry. World origin is centered on the grid so the dungeon
        //    is laid out around (0,0,0) on the XZ plane (top-down view).
        const float cellSize = _params.cellSize;
        const float wallHeight = _params.wallHeight;

        const float originX = -(float)gridWidth * cellSize * 0.5f;
        const float originZ = -(float)gridHeight * cellSize * 0.5f;

        // Cliff/rock detail derived from params.
        const uint32_t wallSegments = _params.wallSegments;
        const uint32_t wallHeightSegments = _params.wallHeightSegments;
        const float wallBump = _params.wallRoughness * cellSize;
        const float topJitter = _params.topRoughness * wallHeight;
        const float wallWander = _params.wallEdgeWander * cellSize;
        const float wallSkirt = _params.wallSkirt * cellSize;

        // Floor palette tints per room color variant for a low-poly look.
        const uint32_t floorColors[4] = {
            Detail::packRGBA8( 0.42f, 0.40f, 0.36f, 1.f ),
            Detail::packRGBA8( 0.38f, 0.36f, 0.40f, 1.f ),
            Detail::packRGBA8( 0.44f, 0.38f, 0.32f, 1.f ),
            Detail::packRGBA8( 0.34f, 0.40f, 0.42f, 1.f ),
        };

        const uint32_t corridorFloorColor = Detail::packRGBA8( 0.30f, 0.29f, 0.31f, 1.f );

        const uint32_t wallTopColor = Detail::packRGBA8( 0.22f, 0.20f, 0.24f, 1.f );
        const uint32_t wallSideColor = Detail::packRGBA8( 0.15f, 0.14f, 0.17f, 1.f );

        // Per-cell room lookup for floor coloring.
        Vector<uint8_t> floorVariant;
        floorVariant.resize( grid.size(), 0xFFu );

        for( const Detail::DungeonRoom & room : rooms )
        {
            for( uint32_t z = room.z; z < room.z + room.h && z < gridHeight; ++z )
            {
                for( uint32_t x = room.x; x < room.x + room.w && x < gridWidth; ++x )
                {
                    floorVariant[z * gridWidth + x] = (uint8_t)room.colorVariant;
                }
            }
        }

        result.floorVertices.reserve( (VectorRenderVertex3D::size_type)grid.size() * 4u );
        result.floorIndices.reserve( (VectorRenderIndex32::size_type)grid.size() * 6u );
        result.wallVertices.reserve( (VectorRenderVertex3D::size_type)grid.size() * 4u );
        result.wallIndices.reserve( (VectorRenderIndex32::size_type)grid.size() * 6u );

        // Elevation sampled at a grid CORNER (gx,gz): average of the walkable cells
        // around it. Shared by floor tiles, wall bases and caps -> everything welds
        // at the same height, so multi-level terrain has no cracks.
        auto cornerElev = [&]( int32_t _gx, int32_t _gz ) -> float
        {
            float sum = 0.f;
            float count = 0.f;

            const int32_t cx[4] = { _gx - 1, _gx, _gx - 1, _gx };
            const int32_t cz[4] = { _gz - 1, _gz - 1, _gz, _gz };

            for( uint32_t k = 0; k != 4u; ++k )
            {
                if( cx[k] < 0 || cz[k] < 0 || cx[k] >= (int32_t)gridWidth || cz[k] >= (int32_t)gridHeight )
                {
                    continue;
                }

                uint32_t idx = (uint32_t)cz[k] * gridWidth + (uint32_t)cx[k];

                if( grid[idx] != Detail::DC_FLOOR )
                {
                    continue;
                }

                sum += elev[idx];
                count += 1.f;
            }

            return (count > 0.f) ? (sum / count) : 0.f;
        };

        for( uint32_t z = 0; z != gridHeight; ++z )
        {
            for( uint32_t x = 0; x != gridWidth; ++x )
            {
                const float x0 = originX + (float)x * cellSize;
                const float x1 = x0 + cellSize;
                const float z0 = originZ + (float)z * cellSize;
                const float z1 = z0 + cellSize;

                if( Detail::isFloor( grid, gridWidth, gridHeight, (int32_t)x, (int32_t)z ) == true )
                {
                    // Floor tile. Corner heights come from the shared elevation
                    // field so adjacent tiles (and corridor ramps) weld seamlessly.
                    uint8_t variant = floorVariant[z * gridWidth + x];
                    uint32_t floorColor = (variant < 4u) ? floorColors[variant] : corridorFloorColor;

                    float e00 = cornerElev( (int32_t)x, (int32_t)z );
                    float e10 = cornerElev( (int32_t)x + 1, (int32_t)z );
                    float e11 = cornerElev( (int32_t)x + 1, (int32_t)z + 1 );
                    float e01 = cornerElev( (int32_t)x, (int32_t)z + 1 );

                    Detail::appendQuad( &result.floorVertices, &result.floorIndices
                        , mt::vec3f( x0, e00, z0 )
                        , mt::vec3f( x1, e10, z0 )
                        , mt::vec3f( x1, e11, z1 )
                        , mt::vec3f( x0, e01, z1 )
                        , mt::vec3f( 0.f, 1.f, 0.f )
                        , floorColor );

                    // Occasionally drop a small rock prop on the floor for flavor.
                    if( Detail::hash01( x, z, _params.seed ^ 0xB0Du ) > 0.90f )
                    {
                        float rx = x0 + cellSize * (0.3f + 0.4f * Detail::hash01( x, z, 0x1A2Bu ));
                        float rz = z0 + cellSize * (0.3f + 0.4f * Detail::hash01( z, x, 0x3C4Du ));
                        float rr = cellSize * (0.07f + 0.06f * Detail::hash01( x + 7u, z + 3u, 0x5E6Fu ));
                        float ry = elev[z * gridWidth + x];

                        Detail::appendBoulder( &result.wallVertices, &result.wallIndices
                            , rx, rz, ry, rr, wallSideColor, (x * 2246822519u) ^ (z * 3266489917u) ^ 0xB0u );
                    }

                    continue;
                }

                // Solid cell: only emit a wall block if it borders walkable floor,
                // otherwise it stays invisible bedrock (keeps the mesh light).
                bool borderWest = Detail::isFloor( grid, gridWidth, gridHeight, (int32_t)x - 1, (int32_t)z );
                bool borderEast = Detail::isFloor( grid, gridWidth, gridHeight, (int32_t)x + 1, (int32_t)z );
                bool borderNorth = Detail::isFloor( grid, gridWidth, gridHeight, (int32_t)x, (int32_t)z - 1 );
                bool borderSouth = Detail::isFloor( grid, gridWidth, gridHeight, (int32_t)x, (int32_t)z + 1 );

                if( borderWest == false && borderEast == false && borderNorth == false && borderSouth == false )
                {
                    continue;
                }

                // Floor elevation at each grid corner (averaged from walkable
                // neighbours). Walls rise from and weld to the local terrain height.
                const float be00 = cornerElev( (int32_t)x, (int32_t)z );
                const float be10 = cornerElev( (int32_t)x + 1, (int32_t)z );
                const float be11 = cornerElev( (int32_t)x + 1, (int32_t)z + 1 );
                const float be01 = cornerElev( (int32_t)x, (int32_t)z + 1 );

                // Deterministic corner heights (shared by every touching piece),
                // lifted onto the local floor elevation.
                const float h00 = be00 + Detail::cornerHeight( x, z, wallHeight, topJitter, _params.seed );
                const float h10 = be10 + Detail::cornerHeight( x + 1u, z, wallHeight, topJitter, _params.seed );
                const float h11 = be11 + Detail::cornerHeight( x + 1u, z + 1u, wallHeight, topJitter, _params.seed );
                const float h01 = be01 + Detail::cornerHeight( x, z + 1u, wallHeight, topJitter, _params.seed );

                // Deterministic horizontal wander per grid corner (shared by cap and
                // every wall face so convex corners stay welded).
                const mt::vec2f o00 = Detail::cornerOffset( x, z, wallWander, _params.seed );
                const mt::vec2f o10 = Detail::cornerOffset( x + 1u, z, wallWander, _params.seed );
                const mt::vec2f o11 = Detail::cornerOffset( x + 1u, z + 1u, wallWander, _params.seed );
                const mt::vec2f o01 = Detail::cornerOffset( x, z + 1u, wallWander, _params.seed );

                // Rocky top cap (two triangles since corners differ in height).
                Detail::appendQuad( &result.wallVertices, &result.wallIndices
                    , mt::vec3f( x0 + o00.x, h00, z0 + o00.y )
                    , mt::vec3f( x1 + o10.x, h10, z0 + o10.y )
                    , mt::vec3f( x1 + o11.x, h11, z1 + o11.y )
                    , mt::vec3f( x0 + o01.x, h01, z1 + o01.y )
                    , mt::vec3f( 0.f, 1.f, 0.f )
                    , wallTopColor );

                // Side faces only toward floor neighbors (the visible cross-section).
                // a -> b direction is chosen so the front face points into the room.
                if( borderEast == true )
                {
                    Detail::appendRockWall( &result.wallVertices, &result.wallIndices
                        , mt::vec2f( x1, z0 ), mt::vec2f( x1, z1 )
                        , o10, o11
                        , h10, h11
                        , be10, be11
                        , mt::vec3f( 1.f, 0.f, 0.f ), wallSideColor
                        , wallSegments, wallHeightSegments, wallBump, wallSkirt, Detail::hash01( x, z, 0x1u ) > 0.5f ? 0x10u : 0x20u );
                }

                if( borderWest == true )
                {
                    Detail::appendRockWall( &result.wallVertices, &result.wallIndices
                        , mt::vec2f( x0, z1 ), mt::vec2f( x0, z0 )
                        , o01, o00
                        , h01, h00
                        , be01, be00
                        , mt::vec3f( -1.f, 0.f, 0.f ), wallSideColor
                        , wallSegments, wallHeightSegments, wallBump, wallSkirt, (x * 73856093u) ^ (z * 19349663u) ^ 0x30u );
                }

                if( borderSouth == true )
                {
                    Detail::appendRockWall( &result.wallVertices, &result.wallIndices
                        , mt::vec2f( x1, z1 ), mt::vec2f( x0, z1 )
                        , o11, o01
                        , h11, h01
                        , be11, be01
                        , mt::vec3f( 0.f, 0.f, 1.f ), wallSideColor
                        , wallSegments, wallHeightSegments, wallBump, wallSkirt, (x * 83492791u) ^ (z * 28411u) ^ 0x40u );
                }

                if( borderNorth == true )
                {
                    Detail::appendRockWall( &result.wallVertices, &result.wallIndices
                        , mt::vec2f( x0, z0 ), mt::vec2f( x1, z0 )
                        , o00, o10
                        , h00, h10
                        , be00, be10
                        , mt::vec3f( 0.f, 0.f, -1.f ), wallSideColor
                        , wallSegments, wallHeightSegments, wallBump, wallSkirt, (x * 2654435761u) ^ (z * 40503u) ^ 0x50u );
                }
            }
        }

        // 3b) Outer border frame: ring the whole map with tall rock slabs that
        // extend well past the edges, so the (now much higher) camera never reveals
        // empty space beyond the playfield. A solid bedrock plane underneath fills
        // every see-through gap between rooms and the rim.
        {
            const float mapX0 = originX;
            const float mapX1 = originX + (float)gridWidth * cellSize;
            const float mapZ0 = originZ;
            const float mapZ1 = originZ + (float)gridHeight * cellSize;

            // Reach far enough that a steep, high camera framing the hero near an
            // edge still only ever sees rock, never the void.
            const float extend = cellSize * 24.f;       // how far the rim reaches outward
            const float rimTop = wallHeight * 2.8f;      // taller than interior walls
            const float rimBottom = -wallHeight * 1.8f;  // sink well below the floor
            const float rimJitter = wallHeight * 0.6f;

            const float oX0 = mapX0 - extend;
            const float oX1 = mapX1 + extend;
            const float oZ0 = mapZ0 - extend;
            const float oZ1 = mapZ1 + extend;

            // Bedrock floor: one big dark plane below every walkable floor so the
            // gaps between rooms (solid cells that emit no geometry) and the area
            // out to the rim never show the background. Sits above the rim bottom
            // so the rim's vertical faces still hide its edges.
            const float bedrockY = -wallHeight * 1.2f;
            const uint32_t bedrockColor = Detail::packRGBA8( 0.09f, 0.08f, 0.11f, 1.f );

            Detail::appendQuad( &result.wallVertices, &result.wallIndices
                , mt::vec3f( oX0, bedrockY, oZ0 )
                , mt::vec3f( oX1, bedrockY, oZ0 )
                , mt::vec3f( oX1, bedrockY, oZ1 )
                , mt::vec3f( oX0, bedrockY, oZ1 )
                , mt::vec3f( 0.f, 1.f, 0.f )
                , bedrockColor );

            // North band (above the map), full outer width.
            Detail::appendBorderSlab( &result.wallVertices, &result.wallIndices
                , oX0, oX1, oZ0, mapZ0, rimTop, rimBottom, rimJitter, wallTopColor, wallSideColor, _params.seed ^ 0xAA01u );
            // South band (below the map).
            Detail::appendBorderSlab( &result.wallVertices, &result.wallIndices
                , oX0, oX1, mapZ1, oZ1, rimTop, rimBottom, rimJitter, wallTopColor, wallSideColor, _params.seed ^ 0xAA02u );
            // West band (left, between the two horizontal bands).
            Detail::appendBorderSlab( &result.wallVertices, &result.wallIndices
                , oX0, mapX0, mapZ0, mapZ1, rimTop, rimBottom, rimJitter, wallTopColor, wallSideColor, _params.seed ^ 0xAA03u );
            // East band (right).
            Detail::appendBorderSlab( &result.wallVertices, &result.wallIndices
                , mapX1, oX1, mapZ0, mapZ1, rimTop, rimBottom, rimJitter, wallTopColor, wallSideColor, _params.seed ^ 0xAA04u );
        }

        // 4) Spawn point at the center of the first room (world space, on floor).
        const Detail::DungeonRoom & firstRoom = rooms[0];

        float spawnX = originX + ((float)firstRoom.x + (float)firstRoom.w * 0.5f) * cellSize;
        float spawnZ = originZ + ((float)firstRoom.z + (float)firstRoom.h * 0.5f) * cellSize;

        float spawnY = (float)firstRoom.level * _params.elevationStep;

        result.spawnPosition = mt::vec3f( spawnX, spawnY, spawnZ );

        // 5) Hand the occupancy grid to gameplay code (collision, camera framing).
        result.cells = std::move( grid );
        result.floorElevation = std::move( elev );
        result.gridWidth = gridWidth;
        result.gridHeight = gridHeight;
        result.cellSize = cellSize;
        result.wallHeight = wallHeight;
        result.originX = originX;
        result.originZ = originZ;

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DDungeonBuildResult::isWalkableWorld( float _x, float _z ) const
    {
        if( this->gridWidth == 0u || this->gridHeight == 0u || this->cellSize <= 0.f )
        {
            return false;
        }

        float fx = (_x - this->originX) / this->cellSize;
        float fz = (_z - this->originZ) / this->cellSize;

        if( fx < 0.f || fz < 0.f )
        {
            return false;
        }

        uint32_t gx = (uint32_t)fx;
        uint32_t gz = (uint32_t)fz;

        if( gx >= this->gridWidth || gz >= this->gridHeight )
        {
            return false;
        }

        return this->cells[gz * this->gridWidth + gx] == 1u;
    }
    //////////////////////////////////////////////////////////////////////////
    float Crimsonland3DDungeonBuildResult::floorHeightWorld( float _x, float _z ) const
    {
        if( this->gridWidth == 0u || this->gridHeight == 0u || this->cellSize <= 0.f
            || this->floorElevation.empty() == true )
        {
            return 0.f;
        }

        // Sample at cell centers so bilinear weights blend neighbouring tiles.
        float fx = (_x - this->originX) / this->cellSize - 0.5f;
        float fz = (_z - this->originZ) / this->cellSize - 0.5f;

        auto sampleCell = [this]( int32_t _gx, int32_t _gz ) -> float
        {
            if( _gx < 0 ) _gx = 0;
            if( _gz < 0 ) _gz = 0;
            if( _gx >= (int32_t)this->gridWidth ) _gx = (int32_t)this->gridWidth - 1;
            if( _gz >= (int32_t)this->gridHeight ) _gz = (int32_t)this->gridHeight - 1;

            return this->floorElevation[(uint32_t)_gz * this->gridWidth + (uint32_t)_gx];
        };

        int32_t gx0 = (int32_t)StdMath::floorf( fx );
        int32_t gz0 = (int32_t)StdMath::floorf( fz );

        float tx = fx - (float)gx0;
        float tz = fz - (float)gz0;

        float h00 = sampleCell( gx0, gz0 );
        float h10 = sampleCell( gx0 + 1, gz0 );
        float h01 = sampleCell( gx0, gz0 + 1 );
        float h11 = sampleCell( gx0 + 1, gz0 + 1 );

        float hx0 = h00 + (h10 - h00) * tx;
        float hx1 = h01 + (h11 - h01) * tx;

        return hx0 + (hx1 - hx0) * tz;
    }
    //////////////////////////////////////////////////////////////////////////
}
