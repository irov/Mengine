#include "Crimsonland3DSceneEventReceiver.h"

#include "Crimsonland3DDungeonGenerator.h"

#include "Engine/Mesh3D.h"
#include "Engine/ResourceMesh3D.h"
#include "Engine/PostProcessFx3D.h"
#include "Engine/DirectionalLight3D.h"
#include "Engine/PointLight3D.h"
#include "Engine/BlobShadow3D.h"
#include "Engine/TextField.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "GLTFImporterInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/NodeHelper.h"
#include "Kernel/NodeCast.h"
#include "Kernel/ResourceCast.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/TextParams.h"

#include "Config/StdMath.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

#include "math/vec3.h"

namespace Mengine
{
    namespace Detail
    {
        static mt::vec3f makeCameraDirection( float _pitch, float _yaw )
        {
            float cp = StdMath::cosf( _pitch );
            float sp = StdMath::sinf( _pitch );
            float cy = StdMath::cosf( _yaw );
            float sy = StdMath::sinf( _yaw );

            mt::vec3f dir;
            dir.x = cp * sy;
            dir.y = sp;
            dir.z = cp * cy;

            return dir;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t packRGBA8( float _r, float _g, float _b, float _a )
        {
            auto pack = []( float _v ) -> uint32_t
            {
                if( _v < 0.f ) _v = 0.f;
                else if( _v > 1.f ) _v = 1.f;

                return (uint32_t)(_v * 255.f + 0.5f);
            };

            return pack( _r ) | (pack( _g ) << 8) | (pack( _b ) << 16) | (pack( _a ) << 24);
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f makeCameraUp( const mt::vec3f & _look )
        {
            mt::vec3f worldUp( 0.f, 1.f, 0.f );

            mt::vec3f right;
            mt::cross_v3_v3( &right, _look, worldUp );

            if( mt::sqrlength_v3( right ) < 1e-6f )
            {
                worldUp = mt::vec3f( 0.f, 0.f, 1.f );
                mt::cross_v3_v3( &right, _look, worldUp );
            }

            mt::vec3f up;
            mt::cross_v3_v3( &up, right, _look );

            mt::vec3f upN;
            mt::norm_safe_v3( &upN, up );

            return upN;
        }
        //////////////////////////////////////////////////////////////////////////
        // Maps a gameplay enemy type to a model from the shared Google Drive pack
        // (downloaded into '.downloads/models'). Returns nullptr when there is no
        // model for the type, so the renderer falls back to a tinted cube.
        //////////////////////////////////////////////////////////////////////////
        static const Char * getEnemyModelFile( const Char * _name )
        {
            if( StdString::strcmp( _name, "Grunt" ) == 0 ) return "Orc";
            if( StdString::strcmp( _name, "Runner" ) == 0 ) return "Ninja";
            if( StdString::strcmp( _name, "Spitter" ) == 0 ) return "Demon";
            if( StdString::strcmp( _name, "Bomber" ) == 0 ) return "BlueDemon";
            if( StdString::strcmp( _name, "Brute" ) == 0 ) return "Yeti";
            if( StdString::strcmp( _name, "Splitter" ) == 0 ) return "MushroomKing";
            if( StdString::strcmp( _name, "Spawnling" ) == 0 ) return "Frog";
            if( StdString::strcmp( _name, "Tank" ) == 0 ) return "Dino";

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static ResourceMesh3DPtr createMeshResource( const ConstString & _name, const VectorRenderVertex3D & _vertices, const VectorRenderIndex32 & _indices, const DocumentInterfacePtr & _doc )
        {
            ResourceMesh3DPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMesh3D" ), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "failed to create ResourceMesh3D '%s'"
                , _name.c_str()
            );

            resource->setName( _name );
            resource->setVertices( _vertices );
            resource->setIndices( _indices );

            if( resource->initialize() == false )
            {
                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        // Procedural capsule (hero proxy): a cylinder capped by two hemispheres,
        // standing on its feet at y = 0. Generated as latitude/longitude rings.
        //////////////////////////////////////////////////////////////////////////
        static ResourceMesh3DPtr createCapsuleResource( const ConstString & _name, float _radius, float _cylinderHeight, uint32_t _color, const DocumentInterfacePtr & _doc )
        {
            VectorRenderVertex3D vertices;
            VectorRenderIndex32 indices;

            const uint32_t slices = 16u;   // around
            const uint32_t capRings = 6u;  // per hemisphere

            const float halfCyl = _cylinderHeight * 0.5f;
            const float feetOffset = halfCyl + _radius; // lift so bottom tip sits at y = 0

            // Ring list: bottom hemisphere (-90 -> 0), then top hemisphere (0 -> 90).
            // The seam between them forms the straight cylinder wall automatically.
            Vector<float> ringY;
            Vector<float> ringR;

            for( uint32_t r = 0; r <= capRings; ++r )
            {
                float phi = -StdMath::constant_half_pi + ((float)r / (float)capRings) * StdMath::constant_half_pi;
                ringY.push_back( -halfCyl + _radius * StdMath::sinf( phi ) );
                ringR.push_back( _radius * StdMath::cosf( phi ) );
            }

            for( uint32_t r = 0; r <= capRings; ++r )
            {
                float phi = ((float)r / (float)capRings) * StdMath::constant_half_pi;
                ringY.push_back( halfCyl + _radius * StdMath::sinf( phi ) );
                ringR.push_back( _radius * StdMath::cosf( phi ) );
            }

            const uint32_t ringCount = (uint32_t)ringY.size();

            for( uint32_t r = 0; r != ringCount; ++r )
            {
                float y = ringY[r];
                float rad = ringR[r];

                float centerY = y;

                if( centerY > halfCyl ) centerY = halfCyl;
                else if( centerY < -halfCyl ) centerY = -halfCyl;

                for( uint32_t s = 0; s <= slices; ++s )
                {
                    float theta = ((float)s / (float)slices) * StdMath::constant_two_pi;
                    float cx = StdMath::cosf( theta );
                    float sz = StdMath::sinf( theta );

                    mt::vec3f pos( rad * cx, y + feetOffset, rad * sz );

                    mt::vec3f normalN;
                    mt::norm_safe_v3( &normalN, mt::vec3f( rad * cx, y - centerY, rad * sz ) );

                    RenderVertex3D v;
                    v.position = pos;
                    v.normal = normalN;
                    v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
                    v.color = _color;
                    v.uv[0] = mt::vec2f( (float)s / (float)slices, (float)r / (float)ringCount );
                    v.uv[1] = v.uv[0];
                    v._padding = 0;

                    vertices.push_back( v );
                }
            }

            const uint32_t stride = slices + 1u;

            for( uint32_t r = 0; r + 1u < ringCount; ++r )
            {
                for( uint32_t s = 0; s < slices; ++s )
                {
                    uint32_t v00 = r * stride + s;
                    uint32_t v10 = r * stride + s + 1u;
                    uint32_t v01 = (r + 1u) * stride + s;
                    uint32_t v11 = (r + 1u) * stride + s + 1u;

                    indices.push_back( (RenderIndex32)v00 );
                    indices.push_back( (RenderIndex32)v01 );
                    indices.push_back( (RenderIndex32)v11 );
                    indices.push_back( (RenderIndex32)v00 );
                    indices.push_back( (RenderIndex32)v11 );
                    indices.push_back( (RenderIndex32)v10 );
                }
            }

            return createMeshResource( _name, vertices, indices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        // Procedural unit cube centered at the origin (half-extent 0.5), baked
        // with a flat color and per-face normals for lighting. Combat actors reuse
        // a single cube per color and scale it per-instance via the transformation.
        //////////////////////////////////////////////////////////////////////////
        static ResourceMesh3DPtr createUnitCubeResource( const ConstString & _name, uint32_t _color, const DocumentInterfacePtr & _doc )
        {
            VectorRenderVertex3D vertices;
            VectorRenderIndex32 indices;

            const mt::vec3f faceNormals[6] = {
                { 0.f, 0.f, -1.f }, { 0.f, 0.f, 1.f },
                { -1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f },
                { 0.f, 1.f, 0.f },  { 0.f, -1.f, 0.f }
            };

            // Four corners per face (CCW when viewed from outside).
            const mt::vec3f faceCorners[6][4] = {
                { { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f } }, // -Z
                { { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f } },     // +Z
                { { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f } },  // -X
                { { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f } },      // +X
                { { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f } },      // +Y
                { { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f } }   // -Y
            };

            const mt::vec2f faceUv[4] = { { 0.f, 1.f }, { 1.f, 1.f }, { 1.f, 0.f }, { 0.f, 0.f } };

            for( uint32_t f = 0; f != 6u; ++f )
            {
                uint32_t base = (uint32_t)vertices.size();

                for( uint32_t c = 0; c != 4u; ++c )
                {
                    RenderVertex3D v;
                    v.position = faceCorners[f][c];
                    v.normal = faceNormals[f];
                    v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
                    v.color = _color;
                    v.uv[0] = faceUv[c];
                    v.uv[1] = faceUv[c];
                    v._padding = 0;

                    vertices.push_back( v );
                }

                indices.push_back( (RenderIndex32)(base + 0u) );
                indices.push_back( (RenderIndex32)(base + 1u) );
                indices.push_back( (RenderIndex32)(base + 2u) );
                indices.push_back( (RenderIndex32)(base + 0u) );
                indices.push_back( (RenderIndex32)(base + 2u) );
                indices.push_back( (RenderIndex32)(base + 3u) );
            }

            return createMeshResource( _name, vertices, indices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        // Procedural octahedron ("diamond" / gem) centered at the origin with
        // half-extent 0.5, baked with a flat color and per-face normals. Used for
        // projectiles so bullets read as little spinning gems instead of cubes.
        //////////////////////////////////////////////////////////////////////////
        static ResourceMesh3DPtr createDiamondResource( const ConstString & _name, uint32_t _color, const DocumentInterfacePtr & _doc )
        {
            VectorRenderVertex3D vertices;
            VectorRenderIndex32 indices;

            const mt::vec3f top( 0.f, 0.5f, 0.f );
            const mt::vec3f bottom( 0.f, -0.5f, 0.f );
            const mt::vec3f equator[4] = {
                { 0.5f, 0.f, 0.f }, { 0.f, 0.f, 0.5f }, { -0.5f, 0.f, 0.f }, { 0.f, 0.f, -0.5f }
            };

            const mt::vec2f triUv[3] = { { 0.5f, 0.f }, { 0.f, 1.f }, { 1.f, 1.f } };

            auto appendFace = [&]( const mt::vec3f & _a, const mt::vec3f & _b, const mt::vec3f & _c )
            {
                mt::vec3f faceNormal;
                mt::cross_v3_v3( &faceNormal, _b - _a, _c - _a );

                mt::vec3f normalN;
                mt::norm_safe_v3( &normalN, faceNormal );

                const mt::vec3f corners[3] = { _a, _b, _c };

                uint32_t base = (uint32_t)vertices.size();

                for( uint32_t c = 0; c != 3u; ++c )
                {
                    RenderVertex3D v;
                    v.position = corners[c];
                    v.normal = normalN;
                    v.tangent = mt::vec4f( 1.f, 0.f, 0.f, 1.f );
                    v.color = _color;
                    v.uv[0] = triUv[c];
                    v.uv[1] = triUv[c];
                    v._padding = 0;

                    vertices.push_back( v );
                }

                indices.push_back( (RenderIndex32)(base + 0u) );
                indices.push_back( (RenderIndex32)(base + 1u) );
                indices.push_back( (RenderIndex32)(base + 2u) );
            };

            for( uint32_t e = 0; e != 4u; ++e )
            {
                const mt::vec3f & a = equator[e];
                const mt::vec3f & b = equator[(e + 1u) % 4u];

                // Upper pyramid face (CCW from outside) and lower pyramid face.
                appendFace( top, a, b );
                appendFace( bottom, b, a );
            }

            return createMeshResource( _name, vertices, indices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DSceneEventReceiver::Crimsonland3DSceneEventReceiver()
        : m_scene( nullptr )
        , m_dungeonSpawn( 0.f, 0.f, 0.f )
        , m_heroPosition( 0.f, 0.f, 0.f )
        , m_heroRadius( 34.f )
        , m_heroMoveSpeed( 420.f )
        , m_cameraYaw( 0.f )
        , m_cameraPitch( -1.40f )
        , m_cameraDistance( 7413.f )
        , m_cameraDistanceMin( 6066.f )
        , m_cameraDistanceMax( 10334.f )
        , m_cameraMouseSensitivity( 3.0f )
        , m_cameraFocusHeight( 0.f )
        , m_combatStarted( false )
        , m_heroMoveDir( 0.f, 0.f )
        , m_xpSpin( 0.f )
        , m_levelUpActive( false )
        , m_heroDeathShown( false )
        , m_savedExposure( 1.1f )
        , m_enemyAtlasState( 0u )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Crimsonland3DSceneEventReceiver::~Crimsonland3DSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Crimsonland3D scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->removeDemoInputHandlers_();

        if( m_demoCameraTimepipe != nullptr )
        {
            Helper::removeTimepipe( m_demoCameraTimepipe );
            m_demoCameraTimepipe = nullptr;
        }

        if( m_blobShadow != nullptr )       { m_blobShadow->removeFromParent();       m_blobShadow = nullptr; }
        if( m_pointLight != nullptr )       { m_pointLight->removeFromParent();       m_pointLight = nullptr; }
        if( m_hero != nullptr )             { m_hero->removeFromParent();             m_hero = nullptr; }
        if( m_directionalLight != nullptr ) { m_directionalLight->removeFromParent(); m_directionalLight = nullptr; }
        for( const NodePtr & demoMesh : m_demoMeshes )
        {
            if( demoMesh != nullptr )
            {
                demoMesh->removeFromParent();
            }
        }
        m_demoMeshes.clear();
        if( m_demoCamera != nullptr )       { m_demoCamera->removeFromParent();       m_demoCamera = nullptr; }
        if( m_background != nullptr )       { m_background->removeFromParent();       m_background = nullptr; }
        if( m_postFx != nullptr )           { m_postFx->removeFromParent();           m_postFx = nullptr; }

        this->teardownCombatRender_();

        if( m_combatStarted == true )
        {
            m_combat.finalize();
            m_combatStarted = false;
        }

        for( const ResourcePtr & demoResource : m_demoResources )
        {
            if( demoResource != nullptr )
            {
                demoResource->finalize();
            }
        }
        m_demoResources.clear();
        m_meshTextureResource = nullptr;

        if( m_blobShadowResource != nullptr )
        {
            m_blobShadowResource->finalize();
            m_blobShadowResource = nullptr;
        }

        m_dungeon = Crimsonland3DDungeonBuildResult();

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float screenW = resolution.getWidthF();
        float screenH = resolution.getHeightF();

        NodePtr postFx = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PostProcessFx3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( postFx, "failed to create PostProcessFx3D node" );

        PostProcessFx3D * postFxImpl = Helper::staticNodeCast<PostProcessFx3D *>( postFx.get() );
        postFxImpl->setSize( {screenW, screenH} );
        postFxImpl->setExposure( 1.1f );
        postFxImpl->setVignette( 0.45f, 0.55f );
        postFxImpl->setChromaticOffset( 1.5f );
        postFxImpl->setHitFlash( {1.f, 0.15f, 0.05f}, 0.f );

        m_scene->addChild( postFx );
        m_postFx = postFx;

        SurfaceSolidColorPtr backgroundSurface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( backgroundSurface, "failed to create background SurfaceSolidColor" );

        backgroundSurface->setSolidColor( {0.13f, 0.16f, 0.20f, 1.f} );
        backgroundSurface->setSolidSize( {screenW, screenH} );

        ShapeQuadFixedPtr background = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( background, "failed to create background ShapeQuadFixed" );

        background->setName( STRINGIZE_STRING_LOCAL( "Background" ) );
        background->setSurface( backgroundSurface );

        postFx->addChild( background );
        m_background = background;

        const ResourcePtr & whiteResource = RESOURCE_SERVICE()
            ->getResourceReference( ConstString::none(), STRINGIZE_STRING_LOCAL( "WhitePixel" ) );

        m_meshTextureResource = Helper::staticResourceCast<ResourceImagePtr>( whiteResource );

        if( m_meshTextureResource == nullptr )
        {
            LOGGER_ERROR( "failed to resolve WhitePixel resource for crimsonland3d procedural meshes" );

            return false;
        }

        // Procedurally generate the underground dungeon (Diablo / Dungeon Keeper
        // style top-down cross-section: rooms connected by tunnels).
        Crimsonland3DDungeonParams dungeonParams;
        dungeonParams.gridWidth = 48;
        dungeonParams.gridHeight = 48;
        dungeonParams.cellSize = 120.f;
        dungeonParams.wallHeight = 170.f;
        dungeonParams.roomCount = 14;
        dungeonParams.roomMinSize = 4;
        dungeonParams.roomMaxSize = 9;
        dungeonParams.corridorRadius = 1;
        dungeonParams.seed = 20240501u;
        // Organic cavern walls.
        dungeonParams.wallSegments = 6;
        dungeonParams.wallHeightSegments = 5;
        dungeonParams.wallRoughness = 0.52f;
        dungeonParams.topRoughness = 0.46f;
        dungeonParams.wallEdgeWander = 0.46f;
        dungeonParams.wallSkirt = 0.45f;
        // Multi-level terrain: rooms sit on raised plateaus or sunken pits and
        // corridors ramp smoothly between them.
        dungeonParams.elevationLevels = 4;
        dungeonParams.elevationStep = 90.f;

        Crimsonland3DDungeonBuildResult dungeon = Crimsonland3DDungeonGenerator::generate( dungeonParams );

        m_dungeonSpawn = dungeon.spawnPosition;

        // Hero starts at the spawn room center.
        m_heroPosition = m_dungeonSpawn;

        RenderCameraProjectionPtr demoCamera = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( demoCamera, "failed to create RenderCameraProjection for crimsonland3d demo" );

        demoCamera->setName( STRINGIZE_STRING_LOCAL( "Demo_Camera" ) );

        demoCamera->setCameraFOV( 0.72f );
        demoCamera->setCameraAspect( screenW / screenH );
        demoCamera->setCameraNear( 5.f );
        demoCamera->setCameraFar( 12000.f );
        demoCamera->setCameraRightSign( -1.f );

        m_demoCamera = demoCamera;
        this->frameFollowCamera_( 0.f );

        postFx->addChild( demoCamera );

        auto addDemoMesh = [this, postFx, demoCamera]( const ConstString & _nodeName, const ResourceMesh3DPtr & _resource, const DocumentInterfacePtr & _doc ) -> bool
        {
            if( _resource == nullptr )
            {
                return false;
            }

            m_demoResources.push_back( _resource );

            NodePtr mesh = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( mesh, "failed to create Mesh3D node '%s'"
                , _nodeName.c_str()
            );

            mesh->setName( _nodeName );

            Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( mesh.get() );
            meshImpl->setResourceMesh3D( _resource );
            meshImpl->setResourceImage( m_meshTextureResource );
            meshImpl->setMaterialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueLit" ) );

            mesh->getRender()->setRenderCamera( demoCamera );

            postFx->addChild( mesh );
            m_demoMeshes.push_back( mesh );

            return true;
        };

        if( addDemoMesh( STRINGIZE_STRING_LOCAL( "Dungeon_Floor" )
            , Detail::createMeshResource( STRINGIZE_STRING_LOCAL( "Crimsonland3D_Dungeon_Floor" ), dungeon.floorVertices, dungeon.floorIndices, MENGINE_DOCUMENT_FACTORABLE )
            , MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        if( dungeon.wallIndices.empty() == false )
        {
            if( addDemoMesh( STRINGIZE_STRING_LOCAL( "Dungeon_Walls" )
                , Detail::createMeshResource( STRINGIZE_STRING_LOCAL( "Crimsonland3D_Dungeon_Walls" ), dungeon.wallVertices, dungeon.wallIndices, MENGINE_DOCUMENT_FACTORABLE )
                , MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                return false;
            }
        }

        // Keep the occupancy grid for collision and camera framing; drop the heavy
        // vertex/index buffers now that the meshes own their copies.
        m_dungeon = std::move( dungeon );
        m_dungeon.floorVertices = VectorRenderVertex3D();
        m_dungeon.floorIndices = VectorRenderIndex32();
        m_dungeon.wallVertices = VectorRenderVertex3D();
        m_dungeon.wallIndices = VectorRenderIndex32();

        // Hero proxy: a capsule that stands on the floor at the spawn point.
        ResourceMesh3DPtr heroResource = Detail::createCapsuleResource(
            STRINGIZE_STRING_LOCAL( "Crimsonland3D_Hero" ), m_heroRadius, 90.f
            , Detail::packRGBA8( 0.82f, 0.78f, 0.36f, 1.f ), MENGINE_DOCUMENT_FACTORABLE );

        if( heroResource == nullptr )
        {
            LOGGER_ERROR( "failed to create hero capsule resource" );

            return false;
        }

        m_demoResources.push_back( heroResource );

        NodePtr hero = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( hero, "failed to create hero Mesh3D node" );

        hero->setName( STRINGIZE_STRING_LOCAL( "Hero" ) );

        Mesh3D * heroImpl = Helper::staticNodeCast<Mesh3D *>( hero.get() );
        heroImpl->setResourceMesh3D( heroResource );
        heroImpl->setResourceImage( m_meshTextureResource );
        heroImpl->setMaterialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueLit" ) );

        hero->getRender()->setRenderCamera( demoCamera );
        hero->getTransformation()->setLocalPosition( m_heroPosition );

        postFx->addChild( hero );
        m_hero = hero;

        // Re-frame now that the occupancy grid is available (initial call above
        // ran before the grid was populated).
        this->frameFollowCamera_( 0.f );

        NodePtr directional = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DirectionalLight3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( directional, "failed to create DirectionalLight3D node" );

        DirectionalLight3D * dirImpl = Helper::staticNodeCast<DirectionalLight3D *>( directional.get() );
        dirImpl->setLightDirection( {-0.4f, -1.f, -0.3f} );
        dirImpl->setLightColor( {1.f, 0.95f, 0.85f} );
        dirImpl->setLightIntensity( 1.2f );
        dirImpl->setLightAmbient( {0.18f, 0.2f, 0.25f} );

        postFx->addChild( directional );
        m_directionalLight = directional;

        NodePtr point = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PointLight3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( point, "failed to create PointLight3D node" );

        PointLight3D * pointImpl = Helper::staticNodeCast<PointLight3D *>( point.get() );
        pointImpl->setLightRadius( 520.f );
        pointImpl->setLightColor( {1.f, 0.6f, 0.2f} );
        pointImpl->setLightIntensity( 1.4f );

        TransformationInterface * pointXform = point->getTransformation();
        pointXform->setLocalPosition( {m_dungeonSpawn.x, 260.f, m_dungeonSpawn.z} );

        postFx->addChild( point );
        m_pointLight = point;

        NodePtr blob = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "BlobShadow3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( blob, "failed to create BlobShadow3D node" );

        ResourceImageEmptyPtr blobResource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageEmpty" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( blobResource, "failed to create blob shadow ResourceImageEmpty" );

        blobResource->setName( STRINGIZE_STRING_LOCAL( "Crimsonland3D_BlobShadow" ) );

        if( blobResource->initialize() == false )
        {
            return false;
        }

        m_blobShadowResource = blobResource;

        BlobShadow3D * blobImpl = Helper::staticNodeCast<BlobShadow3D *>( blob.get() );
        blobImpl->setResourceImage( m_blobShadowResource );
        blobImpl->setShadowSize( 180.f, 120.f );
        blobImpl->setShadowColor( 0.f, 0.f, 0.f, 0.35f );

        TransformationInterface * blobXform = blob->getTransformation();
        blobXform->setLocalPosition( {m_dungeonSpawn.x, 1.f, m_dungeonSpawn.z} );

        blob->getRender()->setRenderCamera( demoCamera );

        postFx->addChild( blob );
        m_blobShadow = blob;

        // Start the auto-shooter combat simulation at the hero spawn.
        m_savedExposure = postFxImpl->getExposure();
        m_combat.initialize( &m_dungeon, mt::vec2f( m_heroPosition.x, m_heroPosition.z ), dungeonParams.seed ^ 0x9E3779B9u );
        m_combatStarted = true;

        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_1, true, [this]( const InputKeyEvent & ){
            this->selectUpgrade_( 0u );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_2, true, [this]( const InputKeyEvent & ){
            this->selectUpgrade_( 1u );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_3, true, [this]( const InputKeyEvent & ){
            this->selectUpgrade_( 2u );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_4, true, [postFxImpl]( const InputKeyEvent & ){
            postFxImpl->setVignette( postFxImpl->getVignetteStrength() * 1.25f, postFxImpl->getVignetteSoftness() );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_5, true, [postFxImpl]( const InputKeyEvent & ){
            postFxImpl->setChromaticOffset( postFxImpl->getChromaticOffset() - 0.5f );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_6, true, [postFxImpl]( const InputKeyEvent & ){
            postFxImpl->setChromaticOffset( postFxImpl->getChromaticOffset() + 0.5f );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        this->addDemoInputHandler_( Helper::addGlobalKeyHandler( KC_SPACE, true, [postFxImpl]( const InputKeyEvent & ){
            postFxImpl->setHitFlash( {1.f, 0.15f, 0.05f}, 0.6f );
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        this->addDemoInputHandler_( Helper::addGlobalMouseMoveHandler( [this]( const InputMouseMoveEvent & _event ){
            this->handleOrbitMouseMove_( _event );
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        // Left-click selects an upgrade card during a level-up.
        this->addDemoInputHandler_( Helper::addGlobalMouseButtonHandler( MC_LBUTTON, true, [this]( const InputMouseButtonEvent & _event ){
            const Resolution & resolution = APPLICATION_SERVICE()
                ->getContentResolution();

            mt::vec2f screen( _event.position.screen.x * resolution.getWidthF(), _event.position.screen.y * resolution.getHeightF() );

            this->handleLevelUpClick_( screen );
        }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_demoCameraTimepipe = Helper::addTimepipe( [this]( const UpdateContext * _context ){
            this->updateGameplay_( _context );
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::updateGameplay_( const UpdateContext * _context )
    {
        float dt = _context->time * 0.001f;

        if( dt <= 0.f )
        {
            return;
        }

        if( dt > 0.1f )
        {
            dt = 0.1f;
        }

        m_xpSpin += dt * 1.6f;

        // Hero death freezes the simulation under a dark overlay.
        if( m_combatStarted == true && m_combat.isHeroDead() == true )
        {
            if( m_heroDeathShown == false )
            {
                this->showHeroDeath_();
            }

            this->syncCombatRender_();

            return;
        }

        // Level-up pauses gameplay; the player picks an upgrade with keys 1/2/3.
        if( m_combatStarted == true && m_combat.isLevelUpPending() == true )
        {
            if( m_levelUpActive == false )
            {
                this->showLevelUp_();
            }

            this->syncCombatRender_();

            return;
        }

        if( m_levelUpActive == true )
        {
            // Resumed without a pending choice (defensive cleanup).
            this->hideLevelUp_();
        }

        this->moveHero_( dt );
        this->frameFollowCamera_( dt );

        if( m_combatStarted == true )
        {
            mt::vec2f heroXZ( m_heroPosition.x, m_heroPosition.z );

            m_combat.update( dt, heroXZ, m_heroMoveDir );

            this->syncCombatRender_();
        }

        // Keep the torch light and the blob shadow glued to the hero.
        if( m_pointLight != nullptr )
        {
            m_pointLight->getTransformation()->setLocalPosition( {m_heroPosition.x, m_heroPosition.y + 260.f, m_heroPosition.z} );
        }

        if( m_blobShadow != nullptr )
        {
            m_blobShadow->getTransformation()->setLocalPosition( {m_heroPosition.x, m_heroPosition.y + 1.f, m_heroPosition.z} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::moveHero_( float _dt )
    {
        if( m_hero == nullptr )
        {
            return;
        }

        // Movement is relative to the camera orientation (top-down feel).
        const mt::vec3f forward3 = Detail::makeCameraDirection( m_cameraPitch, m_cameraYaw );

        mt::vec3f forward;
        mt::norm_safe_v3( &forward, mt::vec3f( forward3.x, 0.f, forward3.z ) );

        mt::vec3f right( forward.z, 0.f, -forward.x );

        mt::vec3f move( 0.f, 0.f, 0.f );

        if( INPUT_SERVICE()->isKeyDown( KC_W ) == true ) { move.x += forward.x; move.z += forward.z; }
        if( INPUT_SERVICE()->isKeyDown( KC_S ) == true ) { move.x -= forward.x; move.z -= forward.z; }
        if( INPUT_SERVICE()->isKeyDown( KC_D ) == true ) { move.x += right.x;   move.z += right.z; }
        if( INPUT_SERVICE()->isKeyDown( KC_A ) == true ) { move.x -= right.x;   move.z -= right.z; }

        if( mt::sqrlength_v3( move ) < 1e-6f )
        {
            m_heroMoveDir = mt::vec2f( 0.f, 0.f );

            return;
        }

        mt::vec3f moveN;
        mt::norm_safe_v3( &moveN, move );

        // Expose the planar move direction for Forward-aimed weapons.
        m_heroMoveDir = mt::vec2f( moveN.x, moveN.z );

        float speed = m_heroMoveSpeed * m_combat.getHeroStats().moveSpeedMul;

        if( INPUT_SERVICE()->isShiftDown() == true )
        {
            speed *= 1.8f;
        }

        const float stepX = moveN.x * speed * _dt;
        const float stepZ = moveN.z * speed * _dt;

        // Keep a healthy gap from the cliffs (don't let the hero hug walls).
        const float probe = m_heroRadius + 26.f;

        // Axis-separated collision so the hero slides along walls.
        float tryX = m_heroPosition.x + stepX;

        if( this->isWalkable_( tryX + (stepX > 0.f ? probe : -probe), m_heroPosition.z ) == true )
        {
            m_heroPosition.x = tryX;
        }

        float tryZ = m_heroPosition.z + stepZ;

        if( this->isWalkable_( m_heroPosition.x, tryZ + (stepZ > 0.f ? probe : -probe) ) == true )
        {
            m_heroPosition.z = tryZ;
        }

        // Follow the terrain height so the hero walks up plateaus and down pits.
        m_heroPosition.y = m_dungeon.floorHeightWorld( m_heroPosition.x, m_heroPosition.z );

        m_hero->getTransformation()->setLocalPosition( m_heroPosition );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::isWalkable_( float _x, float _z ) const
    {
        return m_dungeon.isWalkableWorld( _x, _z );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::frameFollowCamera_( float _dt )
    {
        if( m_demoCamera == nullptr )
        {
            return;
        }

        // Adaptive zoom: sample the grid around the hero. Lots of open floor ->
        // pull the camera out; lots of walls -> push it in (closer / lower).
        const float sampleRadius = 4.5f * m_dungeon.cellSize;
        const int32_t steps = 5;

        float openCount = 0.f;
        float totalCount = 0.f;

        for( int32_t iz = -steps; iz <= steps; ++iz )
        {
            for( int32_t ix = -steps; ix <= steps; ++ix )
            {
                float sx = m_heroPosition.x + (float)ix / (float)steps * sampleRadius;
                float sz = m_heroPosition.z + (float)iz / (float)steps * sampleRadius;

                totalCount += 1.f;

                if( m_dungeon.isWalkableWorld( sx, sz ) == true )
                {
                    openCount += 1.f;
                }
            }
        }

        float openness = (totalCount > 0.f) ? (openCount / totalCount) : 0.5f;

        float targetDistance = m_cameraDistanceMin + (m_cameraDistanceMax - m_cameraDistanceMin) * openness;

        if( _dt <= 0.f )
        {
            m_cameraDistance = targetDistance;
        }
        else
        {
            float t = _dt * 2.5f;

            if( t > 1.f )
            {
                t = 1.f;
            }

            m_cameraDistance += (targetDistance - m_cameraDistance) * t;
        }

        // Smoothly track the hero's terrain height so the camera glides up and
        // down across elevation changes instead of snapping with the floor.
        float targetFocusHeight = m_heroPosition.y;

        if( _dt <= 0.f )
        {
            m_cameraFocusHeight = targetFocusHeight;
        }
        else
        {
            float th = _dt * 3.0f;

            if( th > 1.f )
            {
                th = 1.f;
            }

            m_cameraFocusHeight += (targetFocusHeight - m_cameraFocusHeight) * th;
        }

        const mt::vec3f target( m_heroPosition.x, m_cameraFocusHeight + 110.f, m_heroPosition.z );

        const mt::vec3f forward = Detail::makeCameraDirection( m_cameraPitch, m_cameraYaw );

        const mt::vec3f cameraPosition(
            target.x - forward.x * m_cameraDistance,
            target.y - forward.y * m_cameraDistance,
            target.z - forward.z * m_cameraDistance );

        const mt::vec3f cameraUp = Detail::makeCameraUp( forward );

        m_demoCamera->setCameraPosition( cameraPosition );
        m_demoCamera->setCameraDirection( forward );
        m_demoCamera->setCameraUp( -cameraUp );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::handleOrbitMouseMove_( const InputMouseMoveEvent & _event )
    {
        if( m_demoCamera == nullptr )
        {
            return;
        }

        if( _event.touchId != TC_TOUCH0 )
        {
            return;
        }

        if( INPUT_SERVICE()->isMouseButtonDown( MC_RBUTTON ) == false )
        {
            return;
        }

        m_cameraYaw += _event.screenDelta.x * m_cameraMouseSensitivity;
        m_cameraPitch -= _event.screenDelta.y * m_cameraMouseSensitivity;

        // Keep the camera looking downward at the dungeon floor.
        const float pitchMax = -1.05f;
        const float pitchMin = -StdMath::constant_half_pi + 0.03f;

        if( m_cameraPitch > pitchMax )
        {
            m_cameraPitch = pitchMax;
        }
        else if( m_cameraPitch < pitchMin )
        {
            m_cameraPitch = pitchMin;
        }

        if( m_cameraYaw > StdMath::constant_pi )
        {
            m_cameraYaw -= StdMath::constant_two_pi;
        }
        else if( m_cameraYaw < StdMath::constant_negative_pi )
        {
            m_cameraYaw += StdMath::constant_two_pi;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::addDemoInputHandler_( UniqueId _id )
    {
        if( _id == INVALID_UNIQUE_ID )
        {
            return;
        }

        m_demoInputHandlers.emplace_back( _id );
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::removeDemoInputHandlers_()
    {
        for( UniqueId id : m_demoInputHandlers )
        {
            Helper::removeGlobalHandler( id );
        }

        m_demoInputHandlers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMesh3DPtr Crimsonland3DSceneEventReceiver::getCubeResource_( uint32_t _color )
    {
        for( uint32_t i = 0; i != m_cubeColors.size(); ++i )
        {
            if( m_cubeColors[i] == _color )
            {
                return m_cubeResources[i];
            }
        }

        ConstString name = Helper::stringizeStringFormat( "Crimsonland3D_Cube_%u", _color );

        ResourceMesh3DPtr resource = Detail::createUnitCubeResource( name, _color, MENGINE_DOCUMENT_FACTORABLE );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "failed to create combat cube resource for color %u", _color );

            return nullptr;
        }

        m_cubeColors.push_back( _color );
        m_cubeResources.push_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Crimsonland3DSceneEventReceiver::acquireCubeNode_( uint32_t _color )
    {
        NodePtr mesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mesh, "failed to create combat Mesh3D node" );

        Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( mesh.get() );
        // Assign both resources before addChild(): addChild() activates and
        // compiles the node, and Mesh3D::_compile() asserts both are present.
        meshImpl->setResourceMesh3D( this->getCubeResource_( _color ) );
        meshImpl->setResourceImage( m_meshTextureResource );
        meshImpl->setMaterialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueLit" ) );

        mesh->getRender()->setRenderCamera( m_demoCamera );

        m_postFx->addChild( mesh );

        return mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMesh3DPtr Crimsonland3DSceneEventReceiver::getDiamondResource_( uint32_t _color )
    {
        for( uint32_t i = 0; i != m_diamondColors.size(); ++i )
        {
            if( m_diamondColors[i] == _color )
            {
                return m_diamondResources[i];
            }
        }

        ConstString name = Helper::stringizeStringFormat( "Crimsonland3D_Diamond_%u", _color );

        ResourceMesh3DPtr resource = Detail::createDiamondResource( name, _color, MENGINE_DOCUMENT_FACTORABLE );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "failed to create combat diamond resource for color %u", _color );

            return nullptr;
        }

        m_diamondColors.push_back( _color );
        m_diamondResources.push_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Crimsonland3DSceneEventReceiver::acquireDiamondNode_( uint32_t _color )
    {
        NodePtr mesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mesh, "failed to create combat Mesh3D node" );

        Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( mesh.get() );
        // Assign both resources before addChild(): addChild() activates and
        // compiles the node, and Mesh3D::_compile() asserts both are present.
        meshImpl->setResourceMesh3D( this->getDiamondResource_( _color ) );
        meshImpl->setResourceImage( m_meshTextureResource );
        meshImpl->setMaterialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueLit" ) );

        mesh->getRender()->setRenderCamera( m_demoCamera );

        m_postFx->addChild( mesh );

        return mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::ensureEnemyAtlas_()
    {
        // 1 = loaded, 2 = missing (already probed).
        if( m_enemyAtlasState != 0u )
        {
            return m_enemyAtlasState == 1u;
        }

        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        if( fileGroup == nullptr )
        {
            m_enemyAtlasState = 2u;

            return false;
        }

        FilePath atlasPath = Helper::stringizeFilePath( ".downloads/models/Atlas_Monsters.png" );

        if( fileGroup->existFile( atlasPath, true ) == false )
        {
            LOGGER_MESSAGE( "crimsonland3d: enemy atlas '.downloads/models/Atlas_Monsters.png' not found, models will be flat-tinted" );

            m_enemyAtlasState = 2u;

            return false;
        }

        const ConstString & codecType = CODEC_SERVICE()
            ->findCodecType( atlasPath );

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, atlasPath, MENGINE_DOCUMENT_FACTORABLE );

        content->setCodecType( codecType );

        ResourceImageDefaultPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );

        if( resource == nullptr )
        {
            m_enemyAtlasState = 2u;

            return false;
        }

        // Atlas is a 1024x1024 sheet shared by all monster models.
        mt::vec2f atlasSize( 1024.f, 1024.f );

        resource->setName( STRINGIZE_STRING_LOCAL( "Crimsonland3D_EnemyAtlas" ) );
        resource->setContent( content );

        mt::uv4f uv_image;
        mt::uv4f uv_alpha;
        resource->setUV( 0, uv_image );
        resource->setUV( 1, uv_alpha );

        resource->setMaxSize( atlasSize );
        resource->setSize( atlasSize );

        if( resource->initialize() == false )
        {
            LOGGER_ERROR( "crimsonland3d: failed to initialize enemy atlas resource" );

            m_enemyAtlasState = 2u;

            return false;
        }

        m_enemyAtlasResource = resource;
        m_enemyAtlasState = 1u;

        LOGGER_MESSAGE( "crimsonland3d: loaded enemy atlas '.downloads/models/Atlas_Monsters.png'" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::ensureEnemyModel_( uint32_t _defIndex, const CombatEnemyDef & _def )
    {
        // Grow the per-type model tables on demand.
        while( m_enemyModelState.size() <= _defIndex )
        {
            m_enemyModelState.push_back( 0u );
            m_enemyModelResources.push_back( nullptr );
            m_enemyModelResourcesRed.push_back( nullptr );
        }

        // 1 = loaded, 2 = missing (already probed).
        if( m_enemyModelState[_defIndex] != 0u )
        {
            return m_enemyModelState[_defIndex] == 1u;
        }

        GLTFImporterServiceInterface * gltfService = GLTF_IMPORTER_SERVICE();

        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        if( gltfService == nullptr || fileGroup == nullptr )
        {
            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        // Model file lives in the downloaded pack: '.downloads/models/<Model>.gltf'.
        const Char * modelFile = Detail::getEnemyModelFile( _def.name );

        if( modelFile == nullptr )
        {
            LOGGER_MESSAGE( "crimsonland3d: no model mapping for enemy '%s', using cube"
                , _def.name
            );

            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        ConstString relName = Helper::stringizeStringFormat( ".downloads/models/%s.gltf", modelFile );
        FilePath modelPath = Helper::stringizeFilePath( relName.c_str() );

        if( fileGroup->existFile( modelPath, true ) == false )
        {
            LOGGER_MESSAGE( "crimsonland3d: no enemy model for '%s' (expected '%s'), using cube"
                , _def.name
                , modelPath.c_str()
            );

            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        ContentInterfacePtr modelContent = Helper::makeFileContent( fileGroup, modelPath, MENGINE_DOCUMENT_FACTORABLE );

        ResourceMesh3DPtr importResource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMesh3D" ), MENGINE_DOCUMENT_FACTORABLE );

        mt::mat4f preTransform;
        mt::ident_m4( &preTransform );

        if( importResource == nullptr ||
            gltfService->importMesh( modelContent, preTransform, 0u, importResource ) == false )
        {
            LOGGER_ERROR( "crimsonland3d: failed to import enemy model '%s'", modelPath.c_str() );

            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        VectorRenderVertex3D vertices = importResource->getVertices();
        const VectorRenderIndex32 & indices = importResource->getIndices();

        if( vertices.empty() == true || indices.empty() == true )
        {
            LOGGER_ERROR( "crimsonland3d: enemy model '%s' has no geometry", modelPath.c_str() );

            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        // Normalize to a unit-height mesh centered on X/Z and resting at y = 0,
        // so the existing placement math (scale {w,h,w}, y = floor + h*0.5) holds.
        mt::vec3f boundsMin = vertices[0].position;
        mt::vec3f boundsMax = vertices[0].position;

        for( const RenderVertex3D & v : vertices )
        {
            boundsMin.x = (v.position.x < boundsMin.x) ? v.position.x : boundsMin.x;
            boundsMin.y = (v.position.y < boundsMin.y) ? v.position.y : boundsMin.y;
            boundsMin.z = (v.position.z < boundsMin.z) ? v.position.z : boundsMin.z;
            boundsMax.x = (v.position.x > boundsMax.x) ? v.position.x : boundsMax.x;
            boundsMax.y = (v.position.y > boundsMax.y) ? v.position.y : boundsMax.y;
            boundsMax.z = (v.position.z > boundsMax.z) ? v.position.z : boundsMax.z;
        }

        mt::vec3f center = (boundsMin + boundsMax) * 0.5f;
        float height = boundsMax.y - boundsMin.y;

        if( height < 1e-4f )
        {
            height = 1.f;
        }

        float invHeight = 1.f / height;

        // When the atlas texture is available, keep vertices white so the texture
        // shows naturally; otherwise flat-tint by the enemy type color.
        bool useAtlas = this->ensureEnemyAtlas_();
        uint32_t vertexColor = (useAtlas == true) ? 0xFFFFFFFFu : _def.color;

        for( RenderVertex3D & v : vertices )
        {
            // Center on all axes and scale to unit height, matching the unit cube
            // (y in [-0.5, 0.5]) so the shared placement formula works unchanged.
            v.position = (v.position - center) * invHeight;
            v.color = vertexColor;
        }

        ConstString resName = Helper::stringizeStringFormat( "Crimsonland3D_EnemyModel_%u", _defIndex );

        ResourceMesh3DPtr resource = Detail::createMeshResource( resName, vertices, indices, MENGINE_DOCUMENT_FACTORABLE );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "crimsonland3d: failed to create enemy-model resource for '%s'", _def.name );

            m_enemyModelState[_defIndex] = 2u;

            return false;
        }

        m_enemyModelResources[_defIndex] = resource;
        m_enemyModelState[_defIndex] = 1u;

        // Build a red-tinted twin used during the death animation. The atlas
        // material multiplies texture by vertex color, so a deep-red vertex
        // color reddens the corpse (and tints the flat cube fallback too).
        for( RenderVertex3D & v : vertices )
        {
            v.color = 0xFF1414D2u; // packed RGBA8: R=210 G=20 B=20 A=255
        }

        ConstString resNameRed = Helper::stringizeStringFormat( "Crimsonland3D_EnemyModelRed_%u", _defIndex );

        m_enemyModelResourcesRed[_defIndex] = Detail::createMeshResource( resNameRed, vertices, indices, MENGINE_DOCUMENT_FACTORABLE );

        LOGGER_MESSAGE( "crimsonland3d: loaded enemy model '%s' (%u verts, %u indices)"
            , modelPath.c_str()
            , (uint32_t)vertices.size()
            , (uint32_t)indices.size()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Crimsonland3DSceneEventReceiver::acquireEnemyNode_( uint32_t _defIndex )
    {
        NodePtr mesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mesh, "failed to create combat Mesh3D node" );

        Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( mesh.get() );
        // Assign both resources before addChild(): addChild() activates and
        // compiles the node, and Mesh3D::_compile() asserts both are present.
        meshImpl->setResourceMesh3D( m_enemyModelResources[_defIndex] );
        // Use the shared monster atlas when available, else the white pixel.
        const ResourceImagePtr & textureResource = (m_enemyAtlasResource != nullptr)
            ? m_enemyAtlasResource
            : m_meshTextureResource;
        meshImpl->setResourceImage( textureResource );
        meshImpl->setMaterialName( STRINGIZE_STRING_LOCAL( "Material_OpaqueLit" ) );

        mesh->getRender()->setRenderCamera( m_demoCamera );

        m_postFx->addChild( mesh );

        return mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::syncCombatRender_()
    {
        if( m_combatStarted == false || m_postFx == nullptr )
        {
            return;
        }

        // Enemies: a colored box (or imported GLTF model) per def, on the floor.
        const Vector<CombatEnemy> & enemies = m_combat.getEnemies();

        uint32_t enemyActive = 0u;

        for( uint32_t i = 0; i != enemies.size(); ++i )
        {
            const CombatEnemy & e = enemies[i];

            // Render living enemies and corpses that are still playing the death
            // animation (reddening + sinking through the floor).
            bool dying = (e.alive == false && e.dying == true);

            if( e.alive == false && dying == false )
            {
                continue;
            }

            const CombatEnemyDef & def = m_combat.getEnemyDef( e.defIndex );

            // Use the imported GLTF model for this enemy type when available.
            bool useModel = this->ensureEnemyModel_( e.defIndex, def );

            // Composite slot key: dying flag | model/cube kind | defIndex. A pool
            // slot reused for a different enemy/state rebinds mesh + texture when
            // this key changes. (Keying on color alone broke for dying corpses,
            // which all share one red color, and for cube/model texture swaps.)
            uint32_t desiredKey = e.defIndex & 0x3FFFFFFFu;

            if( dying == true )
            {
                desiredKey |= 0x80000000u;
            }

            if( useModel == true )
            {
                desiredKey |= 0x40000000u;
            }

            while( enemyActive >= m_enemyPool.size() )
            {
                NodePtr enemyNode = (useModel == true)
                    ? this->acquireEnemyNode_( e.defIndex )
                    : this->acquireCubeNode_( def.color );

                m_enemyPool.push_back( enemyNode );
                // Sentinel forces the unified rebind below to run once.
                m_enemyPoolKey.push_back( 0xFFFFFFFFu );
            }

            const NodePtr & node = m_enemyPool[enemyActive];

            if( m_enemyPoolKey[enemyActive] != desiredKey )
            {
                Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( node.get() );

                ResourceMesh3DPtr enemyResource;
                ResourceImagePtr enemyImage;

                if( useModel == true )
                {
                    enemyResource = (dying == true)
                        ? m_enemyModelResourcesRed[e.defIndex]
                        : m_enemyModelResources[e.defIndex];
                    enemyImage = (m_enemyAtlasResource != nullptr)
                        ? m_enemyAtlasResource
                        : m_meshTextureResource;
                }
                else
                {
                    const uint32_t deathColor = 0xFF1414D2u;
                    enemyResource = this->getCubeResource_( dying == true ? deathColor : def.color );
                    enemyImage = m_meshTextureResource;
                }

                // Mesh3D bakes geometry/texture into GPU buffers at compile time,
                // so a plain setResource* on an already-compiled node would keep
                // showing the stale mesh. recompile() releases and re-uploads.
                meshImpl->recompile( [meshImpl, enemyResource, enemyImage]()
                {
                    meshImpl->setResourceMesh3D( enemyResource );
                    meshImpl->setResourceImage( enemyImage );

                    return true;
                } );

                m_enemyPoolKey[enemyActive] = desiredKey;
            }

            // Imported monster models read as too small, scale the visual body up.
            float bodySizeMul = (useModel == true) ? 5.f : 1.f;
            float bodyW = def.radius * 1.7f * def.drawScale * bodySizeMul;
            float bodyH = def.radius * 2.6f * def.drawScale * bodySizeMul;

            float floorY = m_dungeon.floorHeightWorld( e.pos.x, e.pos.y );

            // While dying, sink straight down until the corpse is fully buried.
            float sink = 0.f;

            if( dying == true )
            {
                float progress = e.deathTime / COMBAT_ENEMY_DEATH_DURATION;

                if( progress > 1.f )
                {
                    progress = 1.f;
                }

                sink = progress * (bodyH + def.radius);
            }

            TransformationInterface * xform = node->getTransformation();
            xform->setLocalScale( {bodyW, bodyH, bodyW} );
            xform->setLocalPosition( {e.pos.x, floorY + bodyH * 0.5f - sink, e.pos.y} );

            enemyActive += 1u;
        }

        this->hidePoolFrom_( m_enemyPool, enemyActive );

        // Projectiles: small colored cubes floating above the floor.
        const Vector<CombatProjectile> & projectiles = m_combat.getProjectiles();

        uint32_t projActive = 0u;

        for( uint32_t i = 0; i != projectiles.size(); ++i )
        {
            const CombatProjectile & p = projectiles[i];

            if( p.alive == false )
            {
                continue;
            }

            while( projActive >= m_projectilePool.size() )
            {
                m_projectilePool.push_back( this->acquireDiamondNode_( p.color ) );
                m_projectilePoolColor.push_back( p.color );
            }

            const NodePtr & node = m_projectilePool[projActive];

            if( m_projectilePoolColor[projActive] != p.color )
            {
                Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( node.get() );
                ResourceMesh3DPtr diamondResource = this->getDiamondResource_( p.color );

                // Recompile so the new colored geometry is re-uploaded (a plain
                // setResource* on a compiled node keeps the stale buffer).
                meshImpl->recompile( [meshImpl, diamondResource]()
                {
                    meshImpl->setResourceMesh3D( diamondResource );

                    return true;
                } );

                m_projectilePoolColor[projActive] = p.color;
            }

            float size = p.radius * 2.8f;

            float floorY = m_dungeon.floorHeightWorld( p.pos.x, p.pos.y );

            TransformationInterface * xform = node->getTransformation();
            xform->setLocalScale( {size, size, size} );
            xform->setLocalPosition( {p.pos.x, floorY + 70.f, p.pos.y} );
            // Spin the gem so the diamond shape reads in motion.
            xform->setLocalOrientationY( m_xpSpin * 1.6f );

            projActive += 1u;
        }

        this->hidePoolFrom_( m_projectilePool, projActive );

        // XP cubes: spinning gems whose color encodes their value tier.
        const Vector<CombatXpCube> & xpCubes = m_combat.getXpCubes();

        uint32_t xpActive = 0u;

        for( uint32_t i = 0; i != xpCubes.size(); ++i )
        {
            const CombatXpCube & x = xpCubes[i];

            if( x.alive == false )
            {
                continue;
            }

            while( xpActive >= m_xpPool.size() )
            {
                m_xpPool.push_back( this->acquireCubeNode_( x.color ) );
                m_xpPoolColor.push_back( x.color );
            }

            const NodePtr & node = m_xpPool[xpActive];

            if( m_xpPoolColor[xpActive] != x.color )
            {
                Mesh3D * meshImpl = Helper::staticNodeCast<Mesh3D *>( node.get() );
                ResourceMesh3DPtr xpResource = this->getCubeResource_( x.color );

                // Recompile so the new colored geometry is re-uploaded (a plain
                // setResource* on a compiled node keeps the stale buffer).
                meshImpl->recompile( [meshImpl, xpResource]()
                {
                    meshImpl->setResourceMesh3D( xpResource );

                    return true;
                } );

                m_xpPoolColor[xpActive] = x.color;
            }

            float size = 20.f + (float)x.value * 0.18f;

            float floorY = m_dungeon.floorHeightWorld( x.pos.x, x.pos.y );

            TransformationInterface * xform = node->getTransformation();
            xform->setLocalScale( {size, size, size} );
            xform->setLocalPosition( {x.pos.x, floorY + 26.f, x.pos.y} );
            xform->setLocalOrientationX( m_xpSpin );

            xpActive += 1u;
        }

        this->hidePoolFrom_( m_xpPool, xpActive );

        // Weapon HUD overlay (icons + reload speed + ammo pips).
        this->updateWeaponHud_();

        // Stats HUD (HP, XP, level).
        this->updateStatsHud_();
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Crimsonland3DSceneEventReceiver::acquireHudQuad_( SurfaceSolidColorPtr * const _outSurface )
    {
        SurfaceSolidColorPtr surface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        surface->setSolidColor( {1.f, 1.f, 1.f, 1.f} );
        surface->setSolidSize( {10.f, 10.f} );

        NodePtr quad = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        ShapeQuadFixed * shape = Helper::staticNodeCast<ShapeQuadFixed *>( quad.get() );
        shape->setSurface( surface );

        m_postFx->addChild( quad );

        *_outSurface = surface;

        return quad;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::updateStatsHud_()
    {
        // Font is shared with the level-up cards; ensure it is loaded once.
        if( m_dungeonFont == nullptr )
        {
            this->setupFont_();
        }

        const CombatHeroStats & stats = m_combat.getHeroStats();

        const float marginX = 32.f;
        const float topY = 28.f;
        const float barW = 320.f;
        const float hpH = 22.f;
        const float xpH = 12.f;
        const float gap = 8.f;

        // --- HP bar (red) ---
        if( m_hpBarBack == nullptr )
        {
            m_hpBarBack = this->acquireHudQuad_( &m_hpBarBackSurface );
            m_hpBarFill = this->acquireHudQuad_( &m_hpBarFillSurface );
        }

        float hpFrac = (stats.maxHp > 1e-3f) ? (stats.hp / stats.maxHp) : 0.f;

        if( hpFrac < 0.f ) hpFrac = 0.f;
        else if( hpFrac > 1.f ) hpFrac = 1.f;

        m_hpBarBackSurface->setSolidColor( {0.12f, 0.05f, 0.05f, 0.85f} );
        m_hpBarBackSurface->setSolidSize( {barW, hpH} );
        m_hpBarBack->getTransformation()->setLocalPosition( {marginX, topY, 0.f} );

        m_hpBarFillSurface->setSolidColor( {0.85f, 0.18f, 0.18f, 1.f} );
        m_hpBarFillSurface->setSolidSize( {barW * hpFrac, hpH} );
        m_hpBarFill->getTransformation()->setLocalPosition( {marginX, topY, 0.f} );

        // --- XP bar (cyan) ---
        if( m_xpBarBack == nullptr )
        {
            m_xpBarBack = this->acquireHudQuad_( &m_xpBarBackSurface );
            m_xpBarFill = this->acquireHudQuad_( &m_xpBarFillSurface );
        }

        float xpFrac = (stats.xpToNext > 0u) ? ((float)stats.xp / (float)stats.xpToNext) : 0.f;

        if( xpFrac < 0.f ) xpFrac = 0.f;
        else if( xpFrac > 1.f ) xpFrac = 1.f;

        float xpY = topY + hpH + gap;

        m_xpBarBackSurface->setSolidColor( {0.05f, 0.10f, 0.12f, 0.85f} );
        m_xpBarBackSurface->setSolidSize( {barW, xpH} );
        m_xpBarBack->getTransformation()->setLocalPosition( {marginX, xpY, 0.f} );

        m_xpBarFillSurface->setSolidColor( {0.25f, 0.8f, 0.95f, 1.f} );
        m_xpBarFillSurface->setSolidSize( {barW * xpFrac, xpH} );
        m_xpBarFill->getTransformation()->setLocalPosition( {marginX, xpY, 0.f} );

        if( m_dungeonFont == nullptr )
        {
            return;
        }

        // --- HP value text (centered over the HP bar) ---
        if( m_hpTextNode == nullptr )
        {
            m_hpTextNode = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

            TextField * tf = Helper::staticNodeCast<TextField *>( m_hpTextNode.get() );
            tf->setFont( m_dungeonFont );
            tf->setFontColor( Color( 1.f, 1.f, 1.f, 1.f ) );
            tf->setHorizontAlign( ETFHA_CENTER );
            tf->setVerticalAlign( ETFVA_CENTER );
            tf->setPixelsnap( true );
            m_hpTextNode->getTransformation()->setLocalScale( {0.5f, 0.5f, 1.f} );

            m_postFx->addChild( m_hpTextNode );
        }

        {
            TextField * tf = Helper::staticNodeCast<TextField *>( m_hpTextNode.get() );

            Char buffer[64] = {'\0'};
            MENGINE_SNPRINTF( buffer, sizeof( buffer ), "%u / %u", (uint32_t)(stats.hp + 0.5f), (uint32_t)(stats.maxHp + 0.5f) );

            tf->setText( String( buffer ) );

            m_hpTextNode->getTransformation()->setLocalPosition( {marginX + barW * 0.5f, topY + hpH * 0.5f, 0.f} );
        }

        // --- Level text (right of the bars) ---
        if( m_levelTextNode == nullptr )
        {
            m_levelTextNode = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

            TextField * tf = Helper::staticNodeCast<TextField *>( m_levelTextNode.get() );
            tf->setFont( m_dungeonFont );
            tf->setFontColor( Color( 1.f, 0.95f, 0.5f, 1.f ) );
            tf->setHorizontAlign( ETFHA_LEFT );
            tf->setVerticalAlign( ETFVA_CENTER );
            tf->setPixelsnap( true );
            m_levelTextNode->getTransformation()->setLocalScale( {0.7f, 0.7f, 1.f} );

            m_postFx->addChild( m_levelTextNode );
        }

        {
            TextField * tf = Helper::staticNodeCast<TextField *>( m_levelTextNode.get() );

            Char buffer[64] = {'\0'};
            MENGINE_SNPRINTF( buffer, sizeof( buffer ), "LV %u", stats.level );

            tf->setText( String( buffer ) );

            m_levelTextNode->getTransformation()->setLocalPosition( {marginX + barW + 18.f, topY + hpH * 0.5f, 0.f} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::updateWeaponHud_()
    {
        // Ammo pips per weapon slot (the "like-bullets" reload indicator).
        const uint32_t c_pips = 6u;

        // Font is shared with the level-up cards; ensure it is loaded once.
        if( m_dungeonFont == nullptr )
        {
            this->setupFont_();
        }

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float screenH = resolution.getHeightF();

        const Vector<CombatWeapon> & weapons = m_combat.getWeapons();
        uint32_t count = (uint32_t)weapons.size();

        const float marginX = 32.f;
        const float marginBottom = 28.f;
        const float rowH = 64.f;
        const float iconSize = 40.f;
        const float pipW = 18.f;
        const float pipH = 10.f;
        const float pipGap = 5.f;

        for( uint32_t s = 0; s != count; ++s )
        {
            const CombatWeapon & w = weapons[s];
            const CombatWeaponDef & def = m_combat.getWeaponDef( w.defIndex );

            float r = (float)(def.color & 0xFFu) / 255.f;
            float g = (float)((def.color >> 8) & 0xFFu) / 255.f;
            float b = (float)((def.color >> 16) & 0xFFu) / 255.f;

            // Bottom-left stack (newest slot stacks upward).
            float rowY = screenH - marginBottom - rowH * (float)(s + 1u);
            float iconX = marginX;
            float textX = marginX + iconSize + 12.f;

            // --- Weapon icon (colored square). ---
            while( s >= m_hudIconNodes.size() )
            {
                SurfaceSolidColorPtr surface;
                NodePtr node = this->acquireHudQuad_( &surface );
                m_hudIconNodes.push_back( node );
                m_hudIconSurfaces.push_back( surface );
            }

            m_hudIconSurfaces[s]->setSolidColor( {r, g, b, 0.95f} );
            m_hudIconSurfaces[s]->setSolidSize( {iconSize, iconSize} );
            m_hudIconNodes[s]->getTransformation()->setLocalPosition( {iconX, rowY, 0.f} );

            // --- Reload-speed text label (name + interval). ---
            float effectiveInterval = def.fireInterval / (w.fireRateMul > 0.01f ? w.fireRateMul : 0.01f);

            while( s >= m_hudTextNodes.size() )
            {
                NodePtr textNode = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

                if( m_dungeonFont != nullptr )
                {
                    TextField * tf = Helper::staticNodeCast<TextField *>( textNode.get() );
                    tf->setFont( m_dungeonFont );
                    tf->setFontColor( Color( 1.f, 1.f, 1.f, 1.f ) );
                    tf->setHorizontAlign( ETFHA_LEFT );
                    tf->setVerticalAlign( ETFVA_CENTER );
                    tf->setPixelsnap( true );
                    // The shared font is 36px tall; shrink it to a compact HUD size.
                    textNode->getTransformation()->setLocalScale( {0.6f, 0.6f, 1.f} );
                }

                m_postFx->addChild( textNode );
                m_hudTextNodes.push_back( textNode );
                m_hudTextCachedRate.push_back( -1.f );
            }

            // Refresh the text only when the reload speed actually changes.
            if( m_dungeonFont != nullptr && m_hudTextCachedRate[s] != effectiveInterval )
            {
                TextField * tf = Helper::staticNodeCast<TextField *>( m_hudTextNodes[s].get() );

                Char buffer[96] = {'\0'};
                MENGINE_SNPRINTF( buffer, sizeof( buffer ), "%s  %.2fs", def.name, effectiveInterval );

                tf->setText( String( buffer ) );

                m_hudTextCachedRate[s] = effectiveInterval;
            }

            m_hudTextNodes[s]->getTransformation()->setLocalPosition( {textX, rowY + 13.f, 0.f} );

            // --- Ammo pips: fill up as the weapon reloads (revolver-style). ---
            float progress = 1.f;

            if( effectiveInterval > 1e-3f && w.cooldown > 0.f )
            {
                progress = 1.f - w.cooldown / effectiveInterval;
            }

            if( progress < 0.f ) progress = 0.f;
            else if( progress > 1.f ) progress = 1.f;

            uint32_t filled = (uint32_t)(progress * (float)c_pips + 0.001f);

            for( uint32_t pi = 0; pi != c_pips; ++pi )
            {
                uint32_t flat = s * c_pips + pi;

                while( flat >= m_hudPipNodes.size() )
                {
                    SurfaceSolidColorPtr surface;
                    NodePtr node = this->acquireHudQuad_( &surface );
                    m_hudPipNodes.push_back( node );
                    m_hudPipSurfaces.push_back( surface );
                }

                float pipX = textX + (float)pi * (pipW + pipGap);
                float pipY = rowY + 34.f;

                bool lit = pi < filled;

                if( lit == true )
                {
                    m_hudPipSurfaces[flat]->setSolidColor( {r, g, b, 1.f} );
                }
                else
                {
                    m_hudPipSurfaces[flat]->setSolidColor( {0.15f, 0.15f, 0.18f, 0.85f} );
                }

                m_hudPipSurfaces[flat]->setSolidSize( {pipW, pipH} );
                m_hudPipNodes[flat]->getTransformation()->setLocalPosition( {pipX, pipY, 0.f} );
            }
        }

        // Park HUD widgets that belong to no current weapon slot.
        for( uint32_t i = count; i < m_hudIconNodes.size(); ++i )
        {
            m_hudIconNodes[i]->getTransformation()->setLocalPosition( {-100000.f, 0.f, 0.f} );
        }

        for( uint32_t i = count; i < m_hudTextNodes.size(); ++i )
        {
            m_hudTextNodes[i]->getTransformation()->setLocalPosition( {-100000.f, 0.f, 0.f} );
        }

        for( uint32_t i = count * c_pips; i < m_hudPipNodes.size(); ++i )
        {
            m_hudPipNodes[i]->getTransformation()->setLocalPosition( {-100000.f, 0.f, 0.f} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::hidePoolFrom_( Vector<NodePtr> & _pool, uint32_t _activeCount )
    {
        for( uint32_t i = _activeCount; i < _pool.size(); ++i )
        {
            // Park unused nodes far below the dungeon (cheaper than hide toggling).
            _pool[i]->getTransformation()->setLocalPosition( {0.f, -100000.f, 0.f} );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::teardownCombatRender_()
    {
        this->hideLevelUp_();

        auto clearPool = []( Vector<NodePtr> & _pool )
        {
            for( const NodePtr & node : _pool )
            {
                if( node != nullptr )
                {
                    node->removeFromParent();
                }
            }

            _pool.clear();
        };

        clearPool( m_enemyPool );
        clearPool( m_projectilePool );
        clearPool( m_xpPool );

        m_enemyPoolKey.clear();
        m_projectilePoolColor.clear();
        m_xpPoolColor.clear();

        // Weapon HUD widgets.
        clearPool( m_hudIconNodes );
        clearPool( m_hudTextNodes );
        clearPool( m_hudPipNodes );

        m_hudIconSurfaces.clear();
        m_hudTextCachedRate.clear();
        m_hudPipSurfaces.clear();

        // Stats HUD widgets.
        auto clearNode = []( NodePtr & _node )
        {
            if( _node != nullptr )
            {
                _node->removeFromParent();
                _node = nullptr;
            }
        };

        clearNode( m_hpBarBack );
        clearNode( m_hpBarFill );
        clearNode( m_xpBarBack );
        clearNode( m_xpBarFill );
        clearNode( m_levelTextNode );
        clearNode( m_hpTextNode );

        m_hpBarBackSurface = nullptr;
        m_hpBarFillSurface = nullptr;
        m_xpBarBackSurface = nullptr;
        m_xpBarFillSurface = nullptr;

        for( const ResourceMesh3DPtr & resource : m_cubeResources )
        {
            if( resource != nullptr )
            {
                resource->finalize();
            }
        }

        m_cubeResources.clear();
        m_cubeColors.clear();

        for( const ResourceMesh3DPtr & resource : m_diamondResources )
        {
            if( resource != nullptr )
            {
                resource->finalize();
            }
        }

        m_diamondResources.clear();
        m_diamondColors.clear();

        for( const ResourceMesh3DPtr & resource : m_enemyModelResources )
        {
            if( resource != nullptr )
            {
                resource->finalize();
            }
        }

        m_enemyModelResources.clear();
        m_enemyModelState.clear();

        for( const ResourceMesh3DPtr & resource : m_enemyModelResourcesRed )
        {
            if( resource != nullptr )
            {
                resource->finalize();
            }
        }

        m_enemyModelResourcesRed.clear();

        if( m_enemyAtlasResource != nullptr )
        {
            m_enemyAtlasResource->finalize();
            m_enemyAtlasResource = nullptr;
        }

        m_enemyAtlasState = 0u;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Crimsonland3DSceneEventReceiver::setupFont_()
    {
        if( m_dungeonFont != nullptr )
        {
            return true;
        }

        const ConstString c_fontName = STRINGIZE_STRING_LOCAL( "DungeonFont" );

        // The font may already be registered (scene reload); reuse it.
        if( FONT_SERVICE()->existFont( c_fontName, &m_dungeonFont ) == true )
        {
            return true;
        }

        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid get default file group" );

        // Glyph (TTF source) then font (height + glyph reference).
        ContentInterfacePtr glyphsContent = Helper::makeFileContent( fileGroup, Helper::stringizeFilePath( "Glyphs.json" ), MENGINE_DOCUMENT_FACTORABLE );

        if( FONT_SERVICE()->loadGlyphs( glyphsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "crimsonland3d: failed to load 'Glyphs.json'" );

            return false;
        }

        ContentInterfacePtr fontsContent = Helper::makeFileContent( fileGroup, Helper::stringizeFilePath( "Fonts.json" ), MENGINE_DOCUMENT_FACTORABLE );

        if( FONT_SERVICE()->loadFonts( fontsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "crimsonland3d: failed to load 'Fonts.json'" );

            return false;
        }

        if( FONT_SERVICE()->existFont( c_fontName, &m_dungeonFont ) == false )
        {
            LOGGER_ERROR( "crimsonland3d: font 'DungeonFont' not found after load" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::showLevelUp_()
    {
        if( m_levelUpActive == true || m_postFx == nullptr )
        {
            return;
        }

        m_levelUpActive = true;

        this->setupFont_();

        PostProcessFx3D * postFxImpl = Helper::staticNodeCast<PostProcessFx3D *>( m_postFx.get() );
        m_savedExposure = postFxImpl->getExposure();
        postFxImpl->setExposure( m_savedExposure * 0.25f );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float screenW = resolution.getWidthF();
        float screenH = resolution.getHeightF();

        // Full-screen dim overlay.
        SurfaceSolidColorPtr overlaySurface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        overlaySurface->setSolidColor( {0.f, 0.f, 0.f, 0.6f} );
        overlaySurface->setSolidSize( {screenW, screenH} );

        NodePtr overlay = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        ShapeQuadFixed * overlayShape = Helper::staticNodeCast<ShapeQuadFixed *>( overlay.get() );
        overlayShape->setSurface( overlaySurface );

        m_postFx->addChild( overlay );
        m_overlay = overlay;

        // Up to three colored upgrade cards.
        uint32_t count = 0u;
        const CombatUpgradeOption * options = m_combat.getUpgradeOptions( &count );

        const float cardW = 240.f;
        const float cardH = 360.f;
        const float gap = 60.f;
        float totalW = (float)count * cardW + (count > 0u ? (float)(count - 1u) * gap : 0.f);
        float startX = (screenW - totalW) * 0.5f;
        float cardY = (screenH - cardH) * 0.5f;

        LOGGER_MESSAGE( "=== LEVEL UP! choose an upgrade (press 1/2/3) ===" );

        for( uint32_t i = 0; i != count; ++i )
        {
            const CombatUpgradeOption & opt = options[i];

            float r = (float)(opt.color & 0xFFu) / 255.f;
            float g = (float)((opt.color >> 8) & 0xFFu) / 255.f;
            float b = (float)((opt.color >> 16) & 0xFFu) / 255.f;

            SurfaceSolidColorPtr cardSurface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

            cardSurface->setSolidColor( {r, g, b, 0.92f} );
            cardSurface->setSolidSize( {cardW, cardH} );

            NodePtr card = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

            ShapeQuadFixed * cardShape = Helper::staticNodeCast<ShapeQuadFixed *>( card.get() );
            cardShape->setSurface( cardSurface );

            card->getTransformation()->setLocalPosition( {startX + (float)i * (cardW + gap), cardY, 0.f} );

            m_postFx->addChild( card );
            m_levelUpCards.push_back( card );

            // Remember the screen rect so the card can be clicked.
            m_levelUpCardRects.push_back( mt::vec4f( startX + (float)i * (cardW + gap), cardY, cardW, cardH ) );

            // Card label (rarity-prefixed upgrade text). Centered, wrapped.
            if( m_dungeonFont != nullptr )
            {
                NodePtr textNode = PROTOTYPE_SERVICE()
                    ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

                TextField * textField = Helper::staticNodeCast<TextField *>( textNode.get() );

                const float padding = 18.f;

                // TextField treats '%' as a format specifier; escape it as '%%'
                // so the rarity/percent label renders literally.
                String labelText;
                for( const Char * c = opt.label; *c != '\0'; ++c )
                {
                    labelText += *c;

                    if( *c == '%' )
                    {
                        labelText += '%';
                    }
                }

                textField->setFont( m_dungeonFont );
                textField->setText( labelText );
                textField->setFontColor( Color( 0.05f, 0.05f, 0.05f, 1.f ) );
                textField->setWrap( true );
                textField->setMaxLength( cardW - padding * 2.f );
                textField->setHorizontAlign( ETFHA_CENTER );
                textField->setVerticalAlign( ETFVA_CENTER );
                textField->setPixelsnap( true );

                // Anchored at the card center (card origin is bottom-left).
                textNode->getTransformation()->setLocalPosition( {cardW * 0.5f, cardH * 0.5f, 0.f} );

                card->addChild( textNode );
            }

            LOGGER_MESSAGE( "  [%u] %s", i + 1u, opt.label );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::hideLevelUp_()
    {
        if( m_levelUpActive == false )
        {
            return;
        }

        m_levelUpActive = false;

        if( m_postFx != nullptr )
        {
            PostProcessFx3D * postFxImpl = Helper::staticNodeCast<PostProcessFx3D *>( m_postFx.get() );
            postFxImpl->setExposure( m_savedExposure );
        }

        if( m_overlay != nullptr )
        {
            m_overlay->removeFromParent();
            m_overlay = nullptr;
        }

        for( const NodePtr & card : m_levelUpCards )
        {
            if( card != nullptr )
            {
                card->removeFromParent();
            }
        }

        m_levelUpCards.clear();
        m_levelUpCardRects.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::handleLevelUpClick_( const mt::vec2f & _screen )
    {
        if( m_levelUpActive == false )
        {
            return;
        }

        for( uint32_t i = 0; i != m_levelUpCardRects.size(); ++i )
        {
            const mt::vec4f & rect = m_levelUpCardRects[i];

            if( _screen.x >= rect.x && _screen.x <= rect.x + rect.z &&
                _screen.y >= rect.y && _screen.y <= rect.y + rect.w )
            {
                this->selectUpgrade_( i );

                return;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::selectUpgrade_( uint32_t _index )
    {
        if( m_combatStarted == false || m_combat.isLevelUpPending() == false )
        {
            return;
        }

        uint32_t count = 0u;
        m_combat.getUpgradeOptions( &count );

        if( _index >= count )
        {
            return;
        }

        m_combat.applyUpgrade( _index );

        this->hideLevelUp_();
    }
    //////////////////////////////////////////////////////////////////////////
    void Crimsonland3DSceneEventReceiver::showHeroDeath_()
    {
        if( m_heroDeathShown == true )
        {
            return;
        }

        m_heroDeathShown = true;

        LOGGER_MESSAGE( "=== YOU DIED (survived %u level%s) ==="
            , m_combat.getHeroStats().level
            , m_combat.getHeroStats().level == 1u ? "" : "s"
        );

        if( m_postFx == nullptr )
        {
            return;
        }

        PostProcessFx3D * postFxImpl = Helper::staticNodeCast<PostProcessFx3D *>( m_postFx.get() );
        postFxImpl->setExposure( 0.18f );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        SurfaceSolidColorPtr overlaySurface = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );

        overlaySurface->setSolidColor( {0.25f, 0.f, 0.f, 0.55f} );
        overlaySurface->setSolidSize( {resolution.getWidthF(), resolution.getHeightF()} );

        NodePtr overlay = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );

        ShapeQuadFixed * overlayShape = Helper::staticNodeCast<ShapeQuadFixed *>( overlay.get() );
        overlayShape->setSurface( overlaySurface );

        m_postFx->addChild( overlay );
        m_overlay = overlay;
    }
    //////////////////////////////////////////////////////////////////////////
}
