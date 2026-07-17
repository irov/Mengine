#include "Arena3DSceneEventReceiver.h"
#include "Arena3DHelper.h"

#include "Engine/DirectionalLight3D.h"
#include "Engine/Mesh3D.h"
#include "Engine/PointLight3D.h"
#include "Engine/PostProcessFx3D.h"
#include "Engine/SoundEmitter.h"

#include "GLTFImporterInterface.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/SocketServiceInterface.h"
#include "Interface/SocketSystemInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NodeCast.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/Scene.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdMath.h"

#include "math/mat4.h"
#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( Arena3D::WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static float wrapRadians( float _angle )
        {
            constexpr float pi = 3.14159265358979323846f;
            constexpr float twoPi = 6.28318530717958647692f;
            while( _angle > pi )
            {
                _angle -= twoPi;
            }
            while( _angle < -pi )
            {
                _angle += twoPi;
            }
            return _angle;
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * weaponName( Arena3D::WeaponType _weapon )
        {
            static const Char * names[] = {"nailgun", "rocket", "railgun", "plasmagun", "grenade", "shotgun"};
            return names[weaponIndex( _weapon )];
        }
        //////////////////////////////////////////////////////////////////////////
        static const Char * projectileName( Arena3D::WeaponType _weapon )
        {
            static const Char * names[] = {"nail", "rocket", "pellet", "plasma", "grenade", "pellet"};
            return names[weaponIndex( _weapon )];
        }
        //////////////////////////////////////////////////////////////////////////
        static ConstString materialForQuantization( uint8_t _levels )
        {
            switch( _levels )
            {
            case 2: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit2" );
            case 8: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit8" );
            case 0: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLitContinuous" );
                default: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit4" );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Arena3DSceneEventReceiver::Arena3DSceneEventReceiver()
        : m_scene( nullptr )
        , m_accumulatorMs( 0.0 )
        , m_sourceTimeMs( 0 )
        , m_lastPresentedEventId( 0 )
        , m_presentedMatchId( 0 )
        , m_hitFlash( 0.f )
        , m_warningFlash( 0.f )
        , m_stepCameraOffset( 0.f )
        , m_stepCameraTime( 0.f )
        , m_viewModelTransition( ViewModelTransition::Ready )
        , m_displayedWeapon( Arena3D::WeaponType::Nailgun )
        , m_pendingWeapon( Arena3D::WeaponType::Nailgun )
        , m_viewModelTransitionTime( 0.f )
        , m_drawCalls( 0 )
        , m_visibleChunks( 0 )
        , m_visiblePlayers( 0 )
        , m_visibleProjectiles( 0 )
        , m_visibleExplosionClouds( 0 )
        , m_visibleImpactFlashes( 0 )
        , m_visibleDecals( 0 )
        , m_visibleDeathShards( 0 )
        , m_lightQuantization( 0 )
        , m_localPlayerId( 1 )
        , m_networkMode( false )
        , m_debugHud( false )
        , m_collisionDebug( false )
        , m_turretDebug( false )
        , m_noclip( false )
        , m_cursorCaptured( false )
    {
        m_session.reset( 1, UINT64_C(0xA3E4D01920260714) );
        m_previousState = m_session.client().state();
        m_currentState = m_previousState;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Arena3DSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        m_scene = Helper::staticNodeCast<Scene *>( _entity );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        this->unregisterInput_();
        this->setCursorCaptured_( false );

        if( m_timepipe != nullptr )
        {
            Helper::removeTimepipe( m_timepipe );
            m_timepipe = nullptr;
        }

        if( m_networkClient != nullptr )
        {
            m_networkClient->disconnect();
            m_networkClient.reset();
        }

        for( Vector<NodePtr> & pool : m_projectilePools )
        {
            for( const NodePtr & node : pool )
            {
                node->removeFromParent();
            }
            pool.clear();
        }
        for( auto & pool : m_explosionClouds )
        {
            for( ExplosionCloudVisual & cloud : pool )
            {
                if( cloud.node != nullptr )
                {
                    cloud.node->removeFromParent();
                }
                cloud = ExplosionCloudVisual{};
            }
        }
        for( ImpactFlashVisual & flash : m_impactFlashes )
        {
            if( flash.node != nullptr )
            {
                flash.node->removeFromParent();
            }
            flash = ImpactFlashVisual{};
        }
        for( DecalVisual & decal : m_decals )
        {
            if( decal.node != nullptr )
            {
                decal.node->removeFromParent();
            }
            decal.node = nullptr;
            if( decal.resource != nullptr )
            {
                decal.resource->finalize();
            }
            decal = DecalVisual{};
        }
        for( DeathBurstVisual & burst : m_deathBursts )
        {
            for( NodePtr & node : burst.nodes )
            {
                if( node != nullptr )
                {
                    node->removeFromParent();
                    node = nullptr;
                }
            }
            burst = DeathBurstVisual{};
        }
        if( m_hud != nullptr )
        {
            m_hud->finalize();
            m_hud = nullptr;
        }
        for( NodePtr & node : m_viewModels )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( NodePtr & node : m_pickupNodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( NodePtr & node : m_soundEmitters )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( SurfaceSoundPtr & sound : m_sounds )
        {
            sound = nullptr;
        }
        for( NodePtr & node : m_turretBarrelNodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( NodePtr & node : m_turretBaseNodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( NodePtr & node : m_playerNodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }
        for( auto & layer : m_arenaChunkNodes )
        {
            for( NodePtr & node : layer )
            {
                if( node != nullptr )
                {
                    node->removeFromParent();
                    node = nullptr;
                }
            }
        }
        for( NodePtr & node : m_pointLights )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
                node = nullptr;
            }
        }

        const NodePtr nodes[] = {m_skyNode, m_lavaNode, m_directionalLight, m_worldCamera, m_viewModelCamera, m_postProcess};
        for( const NodePtr & node : nodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
            }
        }

        m_skyNode = nullptr;
        m_lavaNode = nullptr;
        m_directionalLight = nullptr;
        m_worldCamera = nullptr;
        m_viewModelCamera = nullptr;
        m_postProcess = nullptr;

        for( const ResourcePtr & resource : m_resources )
        {
            if( resource != nullptr )
            {
                resource->finalize();
            }
        }
        m_resources.clear();
        m_tileTexture = nullptr;
        m_stoneTexture = nullptr;
        m_metalTexture = nullptr;
        m_hazardTexture = nullptr;
        m_emissiveTexture = nullptr;
        m_decalTexture = nullptr;
        m_whiteTexture = nullptr;
        m_skyResource = nullptr;
        m_lavaResource = nullptr;
        m_impactResource = nullptr;
        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMesh3DPtr Arena3DSceneEventReceiver::importMesh_( const Char * _path, const ConstString & _name )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        GLTFImporterServiceInterface * importer = GLTF_IMPORTER_SERVICE();
        if( fileGroup == nullptr || importer == nullptr )
        {
            return nullptr;
        }

        const FilePath path = Helper::stringizeFilePath( _path );
        if( fileGroup->existFile( path, true ) == false )
        {
            LOGGER_ERROR( "Arena3D missing generated model '%s'", _path );
            return nullptr;
        }

        ResourceMesh3DPtr resource = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMesh3D" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( resource, "Arena3D failed to allocate ResourceMesh3D" );

        mt::mat4f identity;
        mt::ident_m4( &identity );
        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );
        if( importer->importMesh( content, identity, 0, resource ) == false )
        {
            return nullptr;
        }
        resource->setName( _name );
        if( resource->initialize() == false )
        {
            return nullptr;
        }
        m_resources.emplace_back( resource );
        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceImagePtr Arena3DSceneEventReceiver::importTexture_( const Char * _path, const ConstString & _name )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        if( fileGroup == nullptr )
        {
            return nullptr;
        }
        const FilePath path = Helper::stringizeFilePath( _path );
        if( fileGroup->existFile( path, true ) == false )
        {
            return nullptr;
        }

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );
        content->setCodecType( CODEC_SERVICE()->findCodecType( path ) );
        ResourceImageDefaultPtr image = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( image, "Arena3D failed to allocate texture '%s'", _path );
        image->setName( _name );
        image->setContent( content );
        mt::uv4f uvImage;
        mt::uv4f uvAlpha;
        image->setUV( 0, uvImage );
        image->setUV( 1, uvAlpha );
        image->setMaxSize( {64.f, 64.f} );
        image->setSize( {64.f, 64.f} );
        if( image->initialize() == false )
        {
            return nullptr;
        }
        m_resources.emplace_back( image );
        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    SurfaceSoundPtr Arena3DSceneEventReceiver::importSound_( const Char * _path, const ConstString & _name, NodePtr * _emitter )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        if( fileGroup == nullptr )
        {
            return nullptr;
        }
        const FilePath path = Helper::stringizeFilePath( _path );
        if( fileGroup->existFile( path, true ) == false )
        {
            return nullptr;
        }

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );
        content->setCodecType( CODEC_SERVICE()->findCodecType( path ) );
        ResourceSoundPtr resource = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( resource, "Arena3D failed to allocate sound '%s'", _path );
        resource->setName( _name );
        resource->setContent( content );
        resource->setStreamable( false );
        resource->setDefaultVolume( 0.55f );
        if( resource->initialize() == false )
        {
            return nullptr;
        }
        m_resources.emplace_back( resource );

        SurfaceSoundPtr surface = Helper::generateSurfaceFactorable<SurfaceSound>( MENGINE_DOCUMENT_FACTORABLE );
        surface->setName( _name );
        surface->setResourceSound( resource );
        surface->getAnimation()->setLoop( false );

        NodePtr emitter = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "SoundEmitter" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::staticNodeCast<SoundEmitter *>( emitter.get() )->setSurfaceSound( surface );
        m_scene->addChild( emitter );
        *_emitter = emitter;
        return surface;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Arena3DSceneEventReceiver::createMeshNode_( const ConstString & _name, const ResourceMesh3DPtr & _mesh,
        const ResourceImagePtr & _image, const ConstString & _material, const RenderCameraProjectionPtr & _camera, Node * _parent )
    {
        NodePtr node = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Mesh3D" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( node, "Arena3D failed to allocate Mesh3D '%s'", _name.c_str() );
        node->setName( _name );
        Mesh3D * mesh = Helper::staticNodeCast<Mesh3D *>( node.get() );
        mesh->setResourceMesh3D( _mesh );
        mesh->setResourceImage( _image );
        mesh->setMaterialName( _material );
        node->getRender()->setRenderCamera( _camera );
        _parent->addChild( node );
        return node;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Arena3DSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        this->initializeNetwork_();
        const Resolution & resolution = APPLICATION_SERVICE()->getContentResolution();
        const float width = resolution.getWidthF();
        const float height = resolution.getHeightF();
        const float aspect = width / height;

        // Quake III Arena's default cg_fov is a horizontal 90 degrees.
        // RenderCameraProjection expects a vertical FOV in radians.
        const float quakeArenaFov = 2.f * StdMath::atanf( 1.f / aspect );

        NodePtr post = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PostProcessFx3D" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( post, "Arena3D failed to create post process" );
        PostProcessFx3D * postFx = Helper::staticNodeCast<PostProcessFx3D *>( post.get() );
        postFx->setMaterialName( STRINGIZE_STRING_LOCAL( "Arena3D_PostProcess" ) );
        postFx->setSize( {width, height} );
        postFx->setExposure( 1.08f );
        postFx->setVignette( 0.10f, 0.78f );
        postFx->setChromaticOffset( 0.05f );
        m_scene->addChild( post );
        m_postProcess = post;

        m_worldCamera = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE );
        m_worldCamera->setCameraFOV( quakeArenaFov );
        m_worldCamera->setCameraAspect( aspect );
        m_worldCamera->setCameraNear( 0.05f );
        m_worldCamera->setCameraFar( 180.f );
        m_worldCamera->setCameraRightSign( -1.f );
        post->addChild( m_worldCamera );

        // Quake III renders the first-person gun as a dedicated depth-hacked
        // presentation layer. Keep this camera fixed so the model behaves as
        // a 3D HUD element instead of rotating through world space.
        m_viewModelCamera = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE );
        m_viewModelCamera->setCameraPosition( {0.f, 0.f, 0.f} );
        m_viewModelCamera->setCameraDirection( {0.f, 0.f, 1.f} );
        m_viewModelCamera->setCameraUp( {0.f, -1.f, 0.f} );
        m_viewModelCamera->setCameraFOV( quakeArenaFov );
        m_viewModelCamera->setCameraAspect( aspect );
        m_viewModelCamera->setCameraNear( 0.01f );
        m_viewModelCamera->setCameraFar( 8.f );
        m_viewModelCamera->setCameraRightSign( -1.f );
        m_scene->addChild( m_viewModelCamera );

        m_tileTexture = this->importTexture_( "generated/textures/tile.png", STRINGIZE_STRING_LOCAL( "Arena3D_Tile" ) );
        m_stoneTexture = this->importTexture_( "generated/textures/stone.png", STRINGIZE_STRING_LOCAL( "Arena3D_Stone" ) );
        m_metalTexture = this->importTexture_( "generated/textures/metal.png", STRINGIZE_STRING_LOCAL( "Arena3D_Metal" ) );
        m_hazardTexture = this->importTexture_( "generated/textures/hazard.png", STRINGIZE_STRING_LOCAL( "Arena3D_Hazard" ) );
        m_emissiveTexture = this->importTexture_( "generated/textures/emissive.png", STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ) );
        m_decalTexture = this->importTexture_( "generated/textures/decal.png", STRINGIZE_STRING_LOCAL( "Arena3D_Decal" ) );
        m_whiteTexture = this->importTexture_( "generated/textures/white.png", STRINGIZE_STRING_LOCAL( "Arena3D_White" ) );
        if( m_tileTexture == nullptr || m_stoneTexture == nullptr || m_metalTexture == nullptr ||
            m_hazardTexture == nullptr || m_emissiveTexture == nullptr || m_decalTexture == nullptr || m_whiteTexture == nullptr ) return false;

        static const Char * arenaLayerNames[] = {"stone", "metal", "emissive"};
        for( size_t layer = 0; layer != ArenaVisualLayerCount; ++layer )
        {
            for( size_t index = 0; index != ArenaChunkCount; ++index )
            {
                Char path[256];
                MENGINE_SNPRINTF( path, 256, "generated/models/arena01/%s_chunk_%zu.glb", arenaLayerNames[layer], index );
                m_arenaChunkResources[layer][index] = this->importMesh_( path,
                    Helper::stringizeStringFormat( "Arena3D_Arena01_%s_Chunk_%u", arenaLayerNames[layer], static_cast<uint32_t>(index) ) );
                if( m_arenaChunkResources[layer][index] == nullptr )
                {
                    return false;
                }
            }
        }
        m_skyResource = this->importMesh_( "generated/models/sky.glb", STRINGIZE_STRING_LOCAL( "Arena3D_Sky" ) );
        if( m_skyResource == nullptr )
        {
            return false;
        }
        m_lavaResource = this->importMesh_( "generated/models/lava.glb", STRINGIZE_STRING_LOCAL( "Arena3D_Lava" ) );
        if( m_lavaResource == nullptr )
        {
            return false;
        }
        m_playerResource = this->importMesh_( "generated/models/player.glb", STRINGIZE_STRING_LOCAL( "Arena3D_Player" ) );
        if( m_playerResource == nullptr )
        {
            return false;
        }
        m_turretBaseResource = this->importMesh_( "generated/models/turrets/base.glb", STRINGIZE_STRING_LOCAL( "Arena3D_TurretBase" ) );
        if( m_turretBaseResource == nullptr )
        {
            return false;
        }

        for( size_t index = 0; index != Arena3D::WeaponTypeCount; ++index )
        {
            const Arena3D::WeaponType weapon = static_cast<Arena3D::WeaponType>(index);
            const ConstString turretName = Helper::stringizeStringFormat( "Arena3D_Turret_%s", Detail::weaponName( weapon ) );
            const ConstString projectileResourceName = Helper::stringizeStringFormat( "Arena3D_Projectile_%s", Detail::weaponName( weapon ) );
            const ConstString weaponResourceName = Helper::stringizeStringFormat( "Arena3D_Weapon_%s", Detail::weaponName( weapon ) );
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/turrets/%s.glb", Detail::weaponName( weapon ) );
            m_turretResources[index] = this->importMesh_( path, turretName );
            MENGINE_SNPRINTF( path, 256, "generated/models/projectiles/%s.glb", Detail::projectileName( weapon ) );
            m_projectileResources[index] = this->importMesh_( path, projectileResourceName );
            MENGINE_SNPRINTF( path, 256, "generated/models/weapons/%s.glb", Detail::weaponName( weapon ) );
            m_weaponResources[index] = this->importMesh_( path, weaponResourceName );
            if( m_turretResources[index] == nullptr || m_projectileResources[index] == nullptr || m_weaponResources[index] == nullptr )
            {
                return false;
            }
        }
        static const Char * pickupNames[] = {"health", "armor", "ammo"};
        for( size_t index = 0; index != 3; ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/pickups/%s.glb", pickupNames[index] );
            m_pickupResources[index] = this->importMesh_( path, Helper::stringizeStringFormat( "Arena3D_Pickup_%s", pickupNames[index] ) );
            if( m_pickupResources[index] == nullptr )
            {
                return false;
            }
        }
        static const Char * explosionNames[] = {"rocket", "plasma", "grenade"};
        for( size_t index = 0; index != ExplosionTypeCount; ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/effects/%s_explosion.glb", explosionNames[index] );
            m_explosionResources[index] = this->importMesh_( path,
                Helper::stringizeStringFormat( "Arena3D_Explosion_%s", explosionNames[index] ) );
            if( m_explosionResources[index] == nullptr )
            {
                return false;
            }
        }
        m_impactResource = this->importMesh_( "generated/models/effects/impact.glb", STRINGIZE_STRING_LOCAL( "Arena3D_Impact" ) );
        if( m_impactResource == nullptr )
        {
            return false;
        }

        static const Char * soundNames[] = {"nailgun_fire", "rocket_fire", "railgun_charge", "railgun_fire", "plasma_fire", "grenade_fire",
            "shotgun_fire", "impact", "pickup", "jump", "landing", "death", "warning"};
        for( size_t index = 0; index != 13; ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/audio/%s.wav", soundNames[index] );
            m_sounds[index] = this->importSound_( path, Helper::stringizeStringFormat( "Arena3D_Sound_%s", soundNames[index] ), &m_soundEmitters[index] );
            if( m_sounds[index] == nullptr )
            {
                return false;
            }
        }

        const ConstString litMaterial = Detail::materialForQuantization( m_lightQuantization );
        m_skyNode = this->createMeshNode_( STRINGIZE_STRING_LOCAL( "Arena3D_Sky" ), m_skyResource, m_whiteTexture,
            STRINGIZE_STRING_LOCAL( "Arena3D_Sky" ), m_worldCamera, post.get() );
        m_lavaNode = this->createMeshNode_( STRINGIZE_STRING_LOCAL( "Arena3D_Lava" ), m_lavaResource, m_hazardTexture,
            STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ), m_worldCamera, post.get() );
        const ResourceImagePtr arenaLayerTextures[] = {m_stoneTexture, m_metalTexture, m_emissiveTexture};
        const ConstString arenaLayerMaterials[] = {litMaterial, litMaterial, STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" )};
        for( size_t layer = 0; layer != ArenaVisualLayerCount; ++layer )
        {
            for( size_t index = 0; index != ArenaChunkCount; ++index )
            {
                m_arenaChunkNodes[layer][index] = this->createMeshNode_(
                    Helper::stringizeStringFormat( "Arena01_%s_Chunk_%u", arenaLayerNames[layer], static_cast<uint32_t>(index) ),
                    m_arenaChunkResources[layer][index], arenaLayerTextures[layer], arenaLayerMaterials[layer], m_worldCamera, post.get() );
            }
        }

        NodePtr directional = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DirectionalLight3D" ), MENGINE_DOCUMENT_FACTORABLE );
        DirectionalLight3D * light = Helper::staticNodeCast<DirectionalLight3D *>( directional.get() );
        light->setLightDirection( {-0.42f, -1.f, -0.24f} );
        light->setLightColor( {1.f, 0.93f, 0.82f} );
        light->setLightIntensity( 0.92f );
        light->setLightAmbient( {0.34f, 0.39f, 0.50f} );
        post->addChild( directional );
        m_directionalLight = directional;

        static const mt::vec3f pointPositions[] = {
            {-20.f, 4.5f, -20.f}, {20.f, 4.5f, -20.f}, {-20.f, 4.5f, 20.f}, {20.f, 4.5f, 20.f}};
        static const mt::vec3f pointColors[] = {
            {0.18f, 0.55f, 1.f}, {1.f, 0.42f, 0.18f}, {0.22f, 1.f, 0.68f}, {0.62f, 0.32f, 1.f}};
        for( size_t index = 0; index != m_pointLights.size(); ++index )
        {
            NodePtr point = PROTOTYPE_SERVICE()->generatePrototype(
                STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PointLight3D" ), MENGINE_DOCUMENT_FACTORABLE );
            PointLight3D * pointLight = Helper::staticNodeCast<PointLight3D *>( point.get() );
            pointLight->setLightRadius( 27.f );
            pointLight->setLightColor( pointColors[index] );
            pointLight->setLightIntensity( 0.62f );
            point->getTransformation()->setLocalPosition( pointPositions[index] );
            post->addChild( point );
            m_pointLights[index] = point;
        }

        for( size_t index = 0; index != Arena3D::MaximumTurrets; ++index )
        {
            const Arena3D::TurretStateData & turret = m_currentState.turrets[index];
            m_turretBaseNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "TurretBase_%u", turret.id ),
                m_turretBaseResource, m_tileTexture, litMaterial, m_worldCamera, post.get() );
            m_turretBarrelNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "TurretBarrel_%u", turret.id ),
                m_turretResources[Detail::weaponIndex( turret.weapon )], m_emissiveTexture,
                STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ), m_worldCamera, m_turretBaseNodes[index].get() );
            m_turretBarrelNodes[index]->getTransformation()->setLocalPosition( {0.f, 0.62f, 0.f} );
        }
        for( size_t index = 0; index != Arena3D::MaximumPlayers; ++index )
        {
            m_playerNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "Player_%u", static_cast<uint32_t>(index) ),
                m_playerResource, m_tileTexture, litMaterial, m_worldCamera, post.get() );
            m_playerNodes[index]->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
        }
        for( size_t index = 0; index != Arena3D::MaximumPickups; ++index )
        {
            const Arena3D::PickupStateData & pickup = m_currentState.pickups[index];
            m_pickupNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "Pickup_%u", pickup.id ),
                m_pickupResources[static_cast<size_t>(pickup.type)], m_emissiveTexture,
                STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ), m_worldCamera, post.get() );
            m_pickupNodes[index]->getTransformation()->setLocalScale( {0.7f, 0.7f, 0.7f} );
        }
        for( size_t type = 0; type != ExplosionTypeCount; ++type )
        {
            for( size_t slot = 0; slot != ExplosionCloudsPerType; ++slot )
            {
                ExplosionCloudVisual & cloud = m_explosionClouds[type][slot];
                cloud.node = this->createMeshNode_( Helper::stringizeStringFormat( "Explosion_%u_%u",
                    static_cast<uint32_t>(type), static_cast<uint32_t>(slot) ), m_explosionResources[type], m_whiteTexture,
                    STRINGIZE_STRING_LOCAL( "Arena3D_Explosion" ), m_worldCamera, post.get() );
                cloud.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                cloud.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
            }
        }
        for( size_t index = 0; index != m_impactFlashes.size(); ++index )
        {
            ImpactFlashVisual & flash = m_impactFlashes[index];
            flash.node = this->createMeshNode_( Helper::stringizeStringFormat( "Impact_%u", static_cast<uint32_t>(index) ),
                m_impactResource, m_whiteTexture, STRINGIZE_STRING_LOCAL( "Arena3D_Impact" ), m_worldCamera, post.get() );
            flash.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
            flash.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
            flash.node->getRender()->setLocalColorRGBA( 1.f, 1.f, 1.f, 0.f );
        }
        for( size_t burstIndex = 0; burstIndex != m_deathBursts.size(); ++burstIndex )
        {
            DeathBurstVisual & burst = m_deathBursts[burstIndex];
            for( size_t shardIndex = 0; shardIndex != burst.nodes.size(); ++shardIndex )
            {
                NodePtr node = this->createMeshNode_( Helper::stringizeStringFormat( "DeathShard_%u_%u", static_cast<uint32_t>(burstIndex), static_cast<uint32_t>(shardIndex) ),
                    m_impactResource, m_tileTexture, litMaterial, m_worldCamera, post.get() );
                node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                node->getRender()->setLocalColorRGBA( 0.7f, 0.12f, 0.06f, 0.f );
                burst.nodes[shardIndex] = node;
            }
        }
        for( size_t index = 0; index != Arena3D::WeaponTypeCount; ++index )
        {
            m_viewModels[index] = this->createMeshNode_( Helper::stringizeStringFormat( "ViewModel_%u", static_cast<uint32_t>(index) ),
                m_weaponResources[index], m_tileTexture, STRINGIZE_STRING_LOCAL( "Arena3D_ViewModel" ), m_viewModelCamera, m_scene );
            m_viewModels[index]->getTransformation()->setLocalScale( {0.36f, 0.36f, 0.36f} );
        }

        m_hud = Helper::makeFactorableUnique<Arena3DHud>( MENGINE_DOCUMENT_FACTORABLE );
        m_hud->initialize( m_scene, width, height );

        this->syncPresentation_( 1.f );
        this->registerInput_();
        this->setCursorCaptured_( true );
        m_timepipe = Helper::addTimepipe(
            [this]( const UpdateContext * context )
            {
                this->update_( context );
            },
            MENGINE_DOCUMENT_FACTORABLE );
        LOGGER_MESSAGE( "Arena3D ready: generated Arena01, %s client", m_networkMode == true ? "dedicated-server network" : "100 Hz deterministic local" );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Arena3DSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::registerButton_( EKeyCode _key, Arena3D::InputButton _button )
    {
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            _key, true,
            [this, _button]( const InputKeyEvent & event )
            {
                if( event.isRepeat == true )
                {
                    return;
                }
                Arena3D::CommandPayload payload;
                payload.button = _button;
                this->submit_( Arena3D::CommandType::ButtonDown, payload );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            _key, false,
            [this, _button]( const InputKeyEvent & event )
            {
                if( event.isRepeat == true )
                {
                    return;
                }
                Arena3D::CommandPayload payload;
                payload.button = _button;
                this->submit_( Arena3D::CommandType::ButtonUp, payload );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::registerInput_()
    {
        this->registerButton_( KC_W, Arena3D::InputButton::Forward );
        this->registerButton_( KC_S, Arena3D::InputButton::Backward );
        this->registerButton_( KC_A, Arena3D::InputButton::Left );
        this->registerButton_( KC_D, Arena3D::InputButton::Right );
        this->registerButton_( KC_SPACE, Arena3D::InputButton::Jump );
        this->registerButton_( KC_LCONTROL, Arena3D::InputButton::Crouch );

        static const EKeyCode keys[] = {KC_1, KC_2, KC_3, KC_4, KC_5, KC_6};
        for( size_t index = 0; index != 6; ++index )
        {
            m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
                keys[index], true,
                [this, index]( const InputKeyEvent & event )
                {
                    if( event.isRepeat == false )
                    {
                        this->selectWeapon_( static_cast<Arena3D::WeaponType>(index) );
                    }
                },
                MENGINE_DOCUMENT_FACTORABLE ) );
        }
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_Q, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    this->cycleWeapon_( -1 );
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_E, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    this->cycleWeapon_( 1 );
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );

        m_inputHandlers.emplace_back( Helper::addGlobalMouseButtonHandler(
            MC_LBUTTON, true,
            [this]( const InputMouseButtonEvent & )
            {
                this->submit_( Arena3D::CommandType::FireDown );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalMouseButtonHandler(
            MC_LBUTTON, false,
            [this]( const InputMouseButtonEvent & )
            {
                this->submit_( Arena3D::CommandType::FireUp );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalMouseMoveHandler(
            [this]( const InputMouseMoveEvent & event )
            {
                this->onMouseMove_( event );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );

        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_ESCAPE, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == true )
                {
                    return;
                }
                if( m_networkMode == true )
                {
                    this->setCursorCaptured_( m_cursorCaptured == false );
                    return;
                }

                const bool paused = m_session.server().state().paused;
                this->submit_( paused ? Arena3D::CommandType::Resume : Arena3D::CommandType::Pause );
                this->setCursorCaptured_( paused );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_F1, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    m_debugHud = !m_debugHud;
                    this->logDiagnostics_();
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_F2, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    m_collisionDebug = !m_collisionDebug;
                    LOGGER_MESSAGE( "Arena3D collision debug: %s", m_collisionDebug ? "on" : "off" );
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_F3, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    m_turretDebug = !m_turretDebug;
                    this->logDiagnostics_();
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_F4, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == true )
                {
                    return;
                }
                m_noclip = !m_noclip;
                Arena3D::CommandPayload payload;
                payload.enabled = m_noclip;
                this->submit_( Arena3D::CommandType::Noclip, payload );
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler(
            KC_F5, true,
            [this]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false )
                {
                    this->submit_( Arena3D::CommandType::Restart );
                }
            },
            MENGINE_DOCUMENT_FACTORABLE ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::unregisterInput_()
    {
        for( UniqueId handler : m_inputHandlers )
        {
            if( handler != INVALID_UNIQUE_ID )
            {
                Helper::removeGlobalHandler( handler );
            }
        }
        m_inputHandlers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::setCursorCaptured_( bool _captured )
    {
        m_cursorCaptured = _captured;

        if( _captured == true )
        {
            APPLICATION_SERVICE()->setCursorMode( false );

            if( PLATFORM_SERVICE()->setCursorCapture( true ) == false )
            {
                LOGGER_WARNING( "Arena3D platform does not support cursor capture" );
            }
        }
        else
        {
            PLATFORM_SERVICE()->setCursorCapture( false );
            APPLICATION_SERVICE()->setCursorMode( true );
        }

        LOGGER_MESSAGE( "Arena3D cursor capture: %s", _captured == true ? "on" : "off" );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::submit_( Arena3D::CommandType _type, const Arena3D::CommandPayload & _payload )
    {
        if( m_networkMode == true )
        {
            if( m_networkClient != nullptr )
            {
                m_networkClient->sendCommand( _type, m_sourceTimeMs, _payload );
            }
            return;
        }

        m_session.submit( m_session.makeCommand( _type, m_sourceTimeMs, _payload ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::initializeNetwork_()
    {
        if( OPTIONS_SERVICE()->hasOption( "arena3d-server", true ) == false )
        {
            return;
        }

        const Char * host = OPTIONS_SERVICE()->getOptionValue( "arena3d-server", "127.0.0.1" );
        const uint32_t portValue = OPTIONS_SERVICE()->getOptionUInt32( "arena3d-port", 27960 );
        const Char * name = OPTIONS_SERVICE()->getOptionValue( "arena3d-name", "human" );
        if( portValue == 0 || portValue > UINT16_MAX )
        {
            LOGGER_ERROR( "Arena3D invalid network port: %u", portValue );
            return;
        }

        SocketAddress address;
        if( SOCKET_SYSTEM()->resolveAddress( host, static_cast<uint16_t>(portValue), ESocketAddressFamily::IPv4, &address ) != ESocketResult::Successful )
        {
            LOGGER_ERROR( "Arena3D cannot resolve server '%s:%u'", host, portValue );
            return;
        }

        Arena3D::NetworkClientPtr client = Helper::makeFactorableUnique<Arena3D::NetworkClient>( MENGINE_DOCUMENT_FACTORABLE, SOCKET_SERVICE() );
        if( client->connect( address, name, Arena3D::NetworkClientKind::Human ) == false )
        {
            LOGGER_ERROR( "Arena3D cannot start network connection to '%s:%u'", host, portValue );
            return;
        }

        m_networkClient = client;
        m_networkMode = true;
        LOGGER_MESSAGE( "Arena3D network client connecting to %s:%u as '%s'", host, portValue, name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Arena3D::Simulation & Arena3DSceneEventReceiver::activeSimulation_() const
    {
        if( m_networkMode == true && m_networkClient != nullptr )
        {
            return m_networkClient->simulation();
        }
        return m_session.client();
    }
    //////////////////////////////////////////////////////////////////////////
    const Arena3D::VectorServerEvent & Arena3DSceneEventReceiver::activeEvents_() const
    {
        if( m_networkMode == true && m_networkClient != nullptr )
        {
            return m_networkClient->events();
        }
        return m_session.server().events();
    }
    //////////////////////////////////////////////////////////////////////////
    const Arena3D::PlayerState & Arena3DSceneEventReceiver::localPlayer_( const Arena3D::SimulationState & _state ) const
    {
        for( const Arena3D::PlayerState & player : _state.players )
        {
            if( player.active == true && player.id == m_localPlayerId )
            {
                return player;
            }
        }

        for( const Arena3D::PlayerState & player : _state.players )
        {
            if( player.active == true )
            {
                return player;
            }
        }

        return _state.players[0];
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::selectWeapon_( Arena3D::WeaponType _weapon )
    {
        Arena3D::CommandPayload payload;
        payload.weapon = _weapon;
        this->submit_( Arena3D::CommandType::SelectWeapon, payload );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::cycleWeapon_( int32_t _direction )
    {
        const Arena3D::PlayerState & player = this->localPlayer_( m_currentState );
        const int32_t weaponCount = Arena3D::WeaponTypeCount;
        const int32_t selected = static_cast<int32_t>(player.selectedWeapon);
        for( int32_t offset = 1; offset != weaponCount + 1; ++offset )
        {
            const int32_t candidate = (selected + _direction * offset + weaponCount * 2) % weaponCount;
            if( player.ammo[static_cast<size_t>(candidate)] == 0 )
            {
                continue;
            }
            this->selectWeapon_( static_cast<Arena3D::WeaponType>(candidate) );
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::onMouseMove_( const InputMouseMoveEvent & _event )
    {
        if( m_cursorCaptured == false || _event.touchId != TC_TOUCH0 )
        {
            return;
        }
        Arena3D::CommandPayload payload;
        const double sensitivity = 2.4;
        const kf_fixed_t yawRadians = ( static_cast<int32_t>(_event.screenDelta.x * sensitivity * KF_FIXED_SCALE) );
        const kf_fixed_t pitchRadians = ( static_cast<int32_t>(_event.screenDelta.y * sensitivity * KF_FIXED_SCALE) );
        payload.lookYaw = kf_sangle16_from_fixed_radians( yawRadians );
        payload.lookPitch = kf_sangle16_from_fixed_radians( pitchRadians );
        if( payload.lookYaw != 0 || payload.lookPitch != 0 )
            this->submit_( Arena3D::CommandType::LookDelta, payload );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::update_( const UpdateContext * _context )
    {
        double elapsed = Mengine::StdAlgorithm::max( 0.0, Mengine::StdAlgorithm::min( 250.0, static_cast<double>(_context->time) ) );
        this->advancePresentationAnimations_( static_cast<float>(elapsed) * 0.001f );
        m_sourceTimeMs += static_cast<uint64_t>(elapsed);
        m_accumulatorMs += elapsed;
        bool stateAdvanced = false;

        if( m_networkMode == true )
        {
            m_networkClient->updateNetwork();
            if( m_networkClient->state() == Arena3D::NetworkClientState::Connected )
            {
                const Arena3D::SimulationState & networkState = m_networkClient->simulation().state();
                const bool firstState = m_localPlayerId != m_networkClient->playerId();
                if( firstState == true || networkState.matchId != m_currentState.matchId )
                {
                    m_localPlayerId = m_networkClient->playerId();
                    m_previousState = networkState;
                    m_currentState = networkState;
                    m_accumulatorMs = 0.0;
                    stateAdvanced = true;
                }
                else
                {
                    m_currentState = networkState;
                }

                while( m_accumulatorMs >= Arena3D::TickMilliseconds )
                {
                    m_previousState = m_currentState;
                    m_networkClient->tick();
                    m_currentState = m_networkClient->simulation().state();
                    stateAdvanced = true;
                    m_accumulatorMs -= Arena3D::TickMilliseconds;
                }
                this->processEvents_();
            }
        }
        else
        {
            while( m_accumulatorMs >= Arena3D::TickMilliseconds )
            {
                m_previousState = m_currentState;
                if( m_session.tick() == false )
                {
                    LOGGER_ERROR( "Arena3D simulation desync at tick %llu", static_cast<unsigned long long>(m_session.server().state().tick) );
                    m_accumulatorMs = 0.0;
                    break;
                }
                m_currentState = m_session.client().state();
                stateAdvanced = true;
                this->processEvents_();
                m_accumulatorMs -= Arena3D::TickMilliseconds;
            }
        }

        const Arena3D::PlayerState & previousPlayer = this->localPlayer_( m_previousState );
        const Arena3D::PlayerState & currentPlayer = this->localPlayer_( m_currentState );
        if( stateAdvanced == true && previousPlayer.grounded == true && currentPlayer.grounded == true )
        {
            const kf_fixed_t stepDelta = kf_fixed_sub( currentPlayer.position.y, previousPlayer.position.y );
            const kf_fixed_t maximumStep = kf_fixed_add( this->activeSimulation_().config().stepHeight, kf_fixed_from_float( 0.02f ) );
            if( kf_fixed_abs( stepDelta ) > kf_fixed_from_float( 0.001f ) && kf_fixed_abs( stepDelta ) <= maximumStep )
            {
                const float maximumOffset = kf_fixed_to_float( this->activeSimulation_().config().stepHeight ) * 2.f;
                m_stepCameraOffset = Mengine::StdAlgorithm::max( -maximumOffset, Mengine::StdAlgorithm::min( maximumOffset,
                    m_stepCameraOffset + kf_fixed_to_float( stepDelta ) ) );
                m_stepCameraTime = 0.2f;
            }
        }

        this->observeWeaponSelection_();
        m_hitFlash = Mengine::StdAlgorithm::max( 0.f, m_hitFlash - static_cast<float>(elapsed) * 0.004f );
        m_warningFlash = Mengine::StdAlgorithm::max( 0.f, m_warningFlash - static_cast<float>(elapsed) * 0.001f );
        this->updateExplosionClouds_( static_cast<float>(elapsed) * 0.001f );
        this->updateImpactEffects_( static_cast<float>(elapsed) * 0.001f );
        this->updateDeathBursts_( static_cast<float>(elapsed) * 0.001f );
        m_hud->update( static_cast<float>(elapsed) * 0.001f, m_worldCamera, m_presentedCameraPosition, m_presentedCameraDirection );
        Helper::staticNodeCast<PostProcessFx3D *>( m_postProcess.get() )->setHitFlash( {1.f, 0.12f, 0.04f}, m_hitFlash );
        const double interpolationPeriod = Arena3D::TickMilliseconds;
        const float alpha = static_cast<float>(Mengine::StdAlgorithm::max( 0.0, Mengine::StdAlgorithm::min( 1.0, m_accumulatorMs / interpolationPeriod ) ));
        this->syncPresentation_( alpha );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::advancePresentationAnimations_( float _seconds )
    {
        if( m_stepCameraTime > 0.f )
        {
            const float previousTime = m_stepCameraTime;
            m_stepCameraTime = Mengine::StdAlgorithm::max( 0.f, m_stepCameraTime - _seconds );
            m_stepCameraOffset *= m_stepCameraTime / previousTime;
        }

        float remaining = _seconds;
        while( remaining > 0.f && m_viewModelTransition != ViewModelTransition::Ready )
        {
            const float duration = m_viewModelTransition == ViewModelTransition::Lowering ? 0.2f : 0.25f;
            const float advance = Mengine::StdAlgorithm::min( remaining, duration - m_viewModelTransitionTime );
            m_viewModelTransitionTime += advance;
            remaining -= advance;
            if( m_viewModelTransitionTime < duration )
            {
                break;
            }

            m_viewModelTransitionTime = 0.f;
            if( m_viewModelTransition == ViewModelTransition::Lowering )
            {
                m_displayedWeapon = m_pendingWeapon;
                m_viewModelTransition = ViewModelTransition::Raising;
            }
            else
            {
                m_viewModelTransition = ViewModelTransition::Ready;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::observeWeaponSelection_()
    {
        const Arena3D::WeaponType selected = this->localPlayer_( m_currentState ).selectedWeapon;
        if( selected == m_pendingWeapon )
        {
            return;
        }

        m_pendingWeapon = selected;
        if( m_viewModelTransition == ViewModelTransition::Lowering )
        {
            return;
        }
        if( selected == m_displayedWeapon )
        {
            return;
        }

        m_viewModelTransition = ViewModelTransition::Lowering;
        m_viewModelTransitionTime = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::processEvents_()
    {
        const uint64_t matchId = m_currentState.matchId;
        if( m_presentedMatchId != matchId )
        {
            m_presentedMatchId = matchId;
            m_lastPresentedEventId = UINT64_MAX;
            m_stepCameraOffset = 0.f;
            m_stepCameraTime = 0.f;
            this->clearImpactEffects_();
            this->clearDeathBursts_();
            m_hud->clearDamageNumbers();
        }
        const Arena3D::PlayerState & localPlayer = this->localPlayer_( m_currentState );
        for( const Arena3D::ServerEvent & event : this->activeEvents_() )
        {
            if( m_lastPresentedEventId != UINT64_MAX && event.id <= m_lastPresentedEventId )
            {
                continue;
            }
            m_lastPresentedEventId = event.id;
            if( event.type == Arena3D::EventType::Shot )
            {
                static const size_t weaponSounds[] = {0, 1, 3, 4, 5, 6};
                this->playSound_( weaponSounds[Detail::weaponIndex( event.weapon )] );
            }
            else if( event.type == Arena3D::EventType::Hit || event.type == Arena3D::EventType::Impact || event.type == Arena3D::EventType::Explosion )
            {
                this->playSound_( 7 );
            }
            else if( event.type == Arena3D::EventType::JumpPad )
            {
                this->playSound_( 9 );
            }
            else if( event.type == Arena3D::EventType::TurretWarning )
            {
                this->playSound_( 12 );
            }
            else if( event.type == Arena3D::EventType::PlayerDeath )
            {
                this->playSound_( 11 );
            }
            else if( event.type == Arena3D::EventType::PlayerRespawned )
            {
                this->playSound_( 8 );
            }
            else if( event.type == Arena3D::EventType::Pickup )
            {
                this->playSound_( 8 );
            }
            if( event.type == Arena3D::EventType::TurretWarning )
            {
                m_warningFlash = 1.f;
            }
            if( event.type == Arena3D::EventType::Explosion )
            {
                this->spawnExplosionCloud_( event );
            }
            if( event.type == Arena3D::EventType::Explosion && event.weapon == Arena3D::WeaponType::GrenadeLauncher )
            {
                this->spawnGrenadeExplosionDecal_( event );
            }
            if( event.type == Arena3D::EventType::Hit || event.type == Arena3D::EventType::Impact )
            {
                this->spawnImpactFlash_( event );
            }
            if( event.type == Arena3D::EventType::Impact && event.weapon == Arena3D::WeaponType::GrenadeLauncher )
            {
                this->rememberGrenadeImpact_( event );
            }
            if( event.type == Arena3D::EventType::Impact && event.weapon != Arena3D::WeaponType::GrenadeLauncher )
            {
                this->spawnDecal_( event );
            }
            if( event.type == Arena3D::EventType::PlayerDeath && event.targetId != localPlayer.id )
            {
                this->spawnDeathBurst_( event );
            }
            if( event.type == Arena3D::EventType::Damage && event.actorId == localPlayer.id && event.targetId != localPlayer.id )
            {
                const Arena3D::PlayerState * target = this->activeSimulation_().findPlayer( event.targetId );
                if( target != nullptr )
                {
                    m_hud->spawnDamageNumber( event, *target );
                }
            }
            if( event.type == Arena3D::EventType::Damage && event.targetId == localPlayer.id )
            {
                m_hitFlash = 0.65f;
                const mt::vec3f delta = Helper::kinefixVec3ToVec3f( kf_vec3_sub( event.position, localPlayer.position ) );
                const float relative = Detail::wrapRadians( StdMath::atan2f( delta.x, delta.z ) - Helper::kinefixAngleToRadians( localPlayer.yaw ) );
                m_hud->showDamageDirection( relative );
            }
            if( event.type == Arena3D::EventType::PlayerDeath && event.targetId == localPlayer.id )
            {
                m_hitFlash = 1.f;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::logDiagnostics_() const
    {
        const Arena3D::Simulation & simulation = this->activeSimulation_();
        const Arena3D::PlayerState & player = this->localPlayer_( m_currentState );
        const Arena3D::StateChecksum checksum = simulation.checksum();
        const uint64_t reconciliations = m_networkMode == true ? m_networkClient->reconciliationCount() : m_session.reconciliationCount();
        LOGGER_MESSAGE( "Arena3D mode=%s tick=%llu crc=%08X hp=%d speed=%.3f draws=%u chunks=%u projectiles=%u pools=%u reconciliations=%llu",
            m_networkMode == true ? "network" : "local", static_cast<unsigned long long>(simulation.state().tick), checksum.full,
            player.health, kf_fixed_to_float( kf_vec3_length( player.velocity ) ),
            m_drawCalls, m_visibleChunks, m_visibleProjectiles,
            static_cast<uint32_t>(m_projectilePools[0].size() + m_projectilePools[1].size() + m_projectilePools[2].size() +
                m_projectilePools[3].size() + m_projectilePools[4].size() + m_projectilePools[5].size()),
            static_cast<unsigned long long>(reconciliations) );
        if( m_turretDebug == true )
        {
            for( const Arena3D::TurretStateData & turret : m_currentState.turrets )
            {
                LOGGER_MESSAGE( "Arena3D turret=%u state=%u hp=%d cooldown=%u", turret.id,
                    static_cast<uint32_t>(turret.state), turret.health, turret.cooldownTicks );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
