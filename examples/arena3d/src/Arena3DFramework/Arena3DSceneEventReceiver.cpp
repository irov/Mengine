#include "Arena3DSceneEventReceiver.h"

#include "Engine/DirectionalLight3D.h"
#include "Engine/Mesh3D.h"
#include "Engine/PostProcessFx3D.h"
#include "Engine/SoundEmitter.h"

#include "GLTFImporterInterface.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
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
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdMath.h"
#include "Config/StdIO.h"

#include "math/mat4.h"
#include "math/uv4.h"

#include <algorithm>
#include <cmath>

namespace Mengine
{
    namespace
    {
        constexpr size_t weaponIndex( Arena3D::WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }

        mt::vec3f toVec3( const Arena3D::FixedVec3 & _value )
        {
            return mt::vec3f(
                static_cast<float>(_value.x.toDouble()),
                static_cast<float>(_value.y.toDouble()),
                static_cast<float>(_value.z.toDouble()) );
        }

        mt::vec3f interpolate( const Arena3D::FixedVec3 & _from, const Arena3D::FixedVec3 & _to, float _alpha )
        {
            const mt::vec3f from = toVec3( _from );
            const mt::vec3f to = toVec3( _to );
            return from + (to - from) * _alpha;
        }

        float angleToRadians( Arena3D::FixedAngle _angle )
        {
            return static_cast<float>(Arena3D::fixedAngleToRadians( _angle ).toDouble());
        }

        float angleToRadians( Arena3D::FixedSignedAngle _angle )
        {
            return static_cast<float>(Arena3D::fixedSignedAngleToRadians( _angle ).toDouble());
        }

        float wrapRadians( float _angle )
        {
            constexpr float pi = 3.14159265358979323846f;
            constexpr float twoPi = 6.28318530717958647692f;
            while( _angle > pi ) _angle -= twoPi;
            while( _angle < -pi ) _angle += twoPi;
            return _angle;
        }

        const Char * weaponName( Arena3D::WeaponType _weapon )
        {
            static const Char * names[] = {"nailgun", "rocket", "railgun", "plasmagun", "grenade", "shotgun"};
            return names[weaponIndex( _weapon )];
        }

        const Char * projectileName( Arena3D::WeaponType _weapon )
        {
            static const Char * names[] = {"nail", "rocket", "pellet", "plasma", "grenade", "pellet"};
            return names[weaponIndex( _weapon )];
        }

        ConstString materialForQuantization( uint8_t _levels )
        {
            switch( _levels )
            {
            case 2: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit2" );
            case 8: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit8" );
            case 0: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLitContinuous" );
            default: return STRINGIZE_STRING_LOCAL( "Arena3D_OpaqueLit4" );
            }
        }
    }

    Arena3DSceneEventReceiver::Arena3DSceneEventReceiver()
        : m_scene( nullptr )
        , m_accumulatorMs( 0.0 )
        , m_sourceTimeMs( 0 )
        , m_lastPresentedEventId( 0 )
        , m_presentedMatchId( 0 )
        , m_hitFlash( 0.f )
        , m_damageIndicatorTime( 0.f )
        , m_warningFlash( 0.f )
        , m_drawCalls( 0 )
        , m_visibleChunks( 0 )
        , m_visibleProjectiles( 0 )
        , m_lightQuantization( 4 )
        , m_debugHud( false )
        , m_collisionDebug( false )
        , m_turretDebug( false )
        , m_noclip( false )
        , m_cursorCaptured( true )
    {
        m_session.reset( 1, UINT64_C(0xA3E4D01920260714) );
        m_previousState = m_session.client().state();
        m_currentState = m_previousState;
    }

    bool Arena3DSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        m_scene = Helper::staticNodeCast<Scene *>( _entity );
        return true;
    }

    void Arena3DSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        this->unregisterInput_();
        APPLICATION_SERVICE()->setCursorMode( true );

        if( m_timepipe != nullptr )
        {
            Helper::removeTimepipe( m_timepipe );
            m_timepipe = nullptr;
        }

        for( Vector<NodePtr> & pool : m_projectilePools )
        {
            for( const NodePtr & node : pool ) node->removeFromParent();
            pool.clear();
        }
        for( NodePtr & node : m_viewModels ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }
        for( NodePtr & node : m_pickupNodes ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }
        for( NodePtr & node : m_soundEmitters ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }
        for( SurfaceSoundPtr & sound : m_sounds ) sound = nullptr;
        for( NodePtr & node : m_turretBarrelNodes ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }
        for( NodePtr & node : m_turretBaseNodes ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }
        for( NodePtr & node : m_arenaChunkNodes ) if( node != nullptr ) { node->removeFromParent(); node = nullptr; }

        const NodePtr nodes[] = {m_crosshairHorizontal, m_crosshairVertical, m_healthFill, m_healthBack, m_armorFill, m_armorBack,
            m_speedFill, m_speedBack, m_damageIndicator,
            m_directionalLight, m_viewCamera, m_worldCamera, m_postProcess};
        for( const NodePtr & node : nodes ) if( node != nullptr ) node->removeFromParent();

        m_crosshairHorizontal = nullptr;
        m_crosshairVertical = nullptr;
        m_healthFill = nullptr;
        m_healthBack = nullptr;
        m_armorFill = nullptr;
        m_armorBack = nullptr;
        m_speedFill = nullptr;
        m_speedBack = nullptr;
        m_damageIndicator = nullptr;
        m_directionalLight = nullptr;
        m_viewCamera = nullptr;
        m_worldCamera = nullptr;
        m_postProcess = nullptr;

        for( const ResourcePtr & resource : m_resources ) if( resource != nullptr ) resource->finalize();
        m_resources.clear();
        m_tileTexture = nullptr;
        m_emissiveTexture = nullptr;
        m_scene = nullptr;
    }

    ResourceMesh3DPtr Arena3DSceneEventReceiver::importMesh_( const Char * _path, const ConstString & _name )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        GLTFImporterServiceInterface * importer = GLTF_IMPORTER_SERVICE();
        if( fileGroup == nullptr || importer == nullptr ) return nullptr;

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
        if( importer->importMesh( content, identity, 0, resource ) == false ) return nullptr;
        resource->setName( _name );
        if( resource->initialize() == false ) return nullptr;
        m_resources.emplace_back( resource );
        return resource;
    }

    ResourceImagePtr Arena3DSceneEventReceiver::importTexture_( const Char * _path, const ConstString & _name )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        if( fileGroup == nullptr ) return nullptr;
        const FilePath path = Helper::stringizeFilePath( _path );
        if( fileGroup->existFile( path, true ) == false ) return nullptr;

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
        if( image->initialize() == false ) return nullptr;
        m_resources.emplace_back( image );
        return image;
    }

    SurfaceSoundPtr Arena3DSceneEventReceiver::importSound_( const Char * _path, const ConstString & _name, NodePtr * _emitter )
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
        if( fileGroup == nullptr ) return nullptr;
        const FilePath path = Helper::stringizeFilePath( _path );
        if( fileGroup->existFile( path, true ) == false ) return nullptr;

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, path, MENGINE_DOCUMENT_FACTORABLE );
        content->setCodecType( CODEC_SERVICE()->findCodecType( path ) );
        ResourceSoundPtr resource = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceSound" ), MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( resource, "Arena3D failed to allocate sound '%s'", _path );
        resource->setName( _name );
        resource->setContent( content );
        resource->setStreamable( false );
        resource->setDefaultVolume( 0.55f );
        if( resource->initialize() == false ) return nullptr;
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

    NodePtr Arena3DSceneEventReceiver::createHudQuad_( const mt::vec2f & _size, const Color & _color, SurfaceSolidColorPtr * _surface )
    {
        SurfaceSolidColorPtr surface = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FACTORABLE );
        surface->setSolidColor( _color );
        surface->setSolidSize( _size );
        ShapeQuadFixedPtr quad = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FACTORABLE );
        quad->setSurface( surface );
        m_scene->addChild( quad );
        *_surface = surface;
        return quad;
    }

    bool Arena3DSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
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
        postFx->setExposure( 1.15f );
        postFx->setVignette( 0.24f, 0.62f );
        postFx->setChromaticOffset( 0.35f );
        m_scene->addChild( post );
        m_postProcess = post;

        m_worldCamera = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE );
        m_worldCamera->setCameraFOV( quakeArenaFov );
        m_worldCamera->setCameraAspect( aspect );
        m_worldCamera->setCameraNear( 0.05f );
        m_worldCamera->setCameraFar( 180.f );
        m_worldCamera->setCameraRightSign( -1.f );
        post->addChild( m_worldCamera );

        m_viewCamera = Helper::generateNodeFactorable<RenderCameraProjection>( MENGINE_DOCUMENT_FACTORABLE );
        m_viewCamera->setCameraFOV( quakeArenaFov );
        m_viewCamera->setCameraAspect( aspect );
        m_viewCamera->setCameraNear( 0.01f );
        m_viewCamera->setCameraFar( 10.f );
        m_viewCamera->setCameraRightSign( -1.f );
        m_viewCamera->setCameraPosition( {0.f, 0.f, 0.f} );
        m_viewCamera->setCameraDirection( {0.f, 0.f, 1.f} );
        m_viewCamera->setCameraUp( {0.f, -1.f, 0.f} );
        post->addChild( m_viewCamera );

        m_tileTexture = this->importTexture_( "generated/textures/tile.png", STRINGIZE_STRING_LOCAL( "Arena3D_Tile" ) );
        m_emissiveTexture = this->importTexture_( "generated/textures/emissive.png", STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ) );
        if( m_tileTexture == nullptr || m_emissiveTexture == nullptr ) return false;

        for( size_t index = 0; index != m_arenaChunkResources.size(); ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/arena01/chunk_%zu.glb", index );
            m_arenaChunkResources[index] = this->importMesh_( path,
                Helper::stringizeStringFormat( "Arena3D_Arena01_Chunk_%u", static_cast<uint32_t>(index) ) );
            if( m_arenaChunkResources[index] == nullptr ) return false;
        }
        m_turretBaseResource = this->importMesh_( "generated/models/turrets/base.glb", STRINGIZE_STRING_LOCAL( "Arena3D_TurretBase" ) );
        if( m_turretBaseResource == nullptr ) return false;

        for( size_t index = 0; index != static_cast<size_t>(Arena3D::WeaponType::Count); ++index )
        {
            const Arena3D::WeaponType weapon = static_cast<Arena3D::WeaponType>(index);
            const ConstString turretName = Helper::stringizeStringFormat( "Arena3D_Turret_%s", weaponName( weapon ) );
            const ConstString projectileResourceName = Helper::stringizeStringFormat( "Arena3D_Projectile_%s", weaponName( weapon ) );
            const ConstString weaponResourceName = Helper::stringizeStringFormat( "Arena3D_Weapon_%s", weaponName( weapon ) );
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/turrets/%s.glb", weaponName( weapon ) );
            m_turretResources[index] = this->importMesh_( path, turretName );
            MENGINE_SNPRINTF( path, 256, "generated/models/projectiles/%s.glb", projectileName( weapon ) );
            m_projectileResources[index] = this->importMesh_( path, projectileResourceName );
            MENGINE_SNPRINTF( path, 256, "generated/models/weapons/%s.glb", weaponName( weapon ) );
            m_weaponResources[index] = this->importMesh_( path, weaponResourceName );
            if( m_turretResources[index] == nullptr || m_projectileResources[index] == nullptr || m_weaponResources[index] == nullptr ) return false;
        }
        static const Char * pickupNames[] = {"health", "armor", "ammo"};
        for( size_t index = 0; index != 3; ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/models/pickups/%s.glb", pickupNames[index] );
            m_pickupResources[index] = this->importMesh_( path, Helper::stringizeStringFormat( "Arena3D_Pickup_%s", pickupNames[index] ) );
            if( m_pickupResources[index] == nullptr ) return false;
        }

        static const Char * soundNames[] = {"nailgun_fire", "rocket_fire", "railgun_charge", "railgun_fire", "plasma_fire", "grenade_fire",
            "shotgun_fire", "impact", "pickup", "jump", "landing", "death", "warning"};
        for( size_t index = 0; index != 13; ++index )
        {
            Char path[256];
            MENGINE_SNPRINTF( path, 256, "generated/audio/%s.wav", soundNames[index] );
            m_sounds[index] = this->importSound_( path, Helper::stringizeStringFormat( "Arena3D_Sound_%s", soundNames[index] ), &m_soundEmitters[index] );
            if( m_sounds[index] == nullptr ) return false;
        }

        const ConstString litMaterial = materialForQuantization( m_lightQuantization );
        for( size_t index = 0; index != m_arenaChunkNodes.size(); ++index )
        {
            m_arenaChunkNodes[index] = this->createMeshNode_(
                Helper::stringizeStringFormat( "Arena01_Chunk_%u", static_cast<uint32_t>(index) ),
                m_arenaChunkResources[index], m_tileTexture, litMaterial, m_worldCamera, post.get() );
        }

        NodePtr directional = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "DirectionalLight3D" ), MENGINE_DOCUMENT_FACTORABLE );
        DirectionalLight3D * light = Helper::staticNodeCast<DirectionalLight3D *>( directional.get() );
        light->setLightDirection( {-0.42f, -1.f, -0.24f} );
        light->setLightColor( {1.f, 0.92f, 0.78f} );
        light->setLightIntensity( 1.25f );
        light->setLightAmbient( {0.18f, 0.23f, 0.31f} );
        post->addChild( directional );
        m_directionalLight = directional;

        for( size_t index = 0; index != Arena3D::MaximumTurrets; ++index )
        {
            const Arena3D::TurretStateData & turret = m_currentState.turrets[index];
            m_turretBaseNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "TurretBase_%u", turret.id ),
                m_turretBaseResource, m_tileTexture, litMaterial, m_worldCamera, post.get() );
            m_turretBarrelNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "TurretBarrel_%u", turret.id ),
                m_turretResources[weaponIndex( turret.weapon )], m_emissiveTexture,
                STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ), m_worldCamera, m_turretBaseNodes[index].get() );
            m_turretBarrelNodes[index]->getTransformation()->setLocalPosition( {0.f, 0.62f, 0.f} );
        }
        for( size_t index = 0; index != Arena3D::MaximumPickups; ++index )
        {
            const Arena3D::PickupStateData & pickup = m_currentState.pickups[index];
            m_pickupNodes[index] = this->createMeshNode_( Helper::stringizeStringFormat( "Pickup_%u", pickup.id ),
                m_pickupResources[static_cast<size_t>(pickup.type)], m_emissiveTexture,
                STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ), m_worldCamera, post.get() );
            m_pickupNodes[index]->getTransformation()->setLocalScale( {0.7f, 0.7f, 0.7f} );
        }

        for( size_t index = 0; index != static_cast<size_t>(Arena3D::WeaponType::Count); ++index )
        {
            m_viewModels[index] = this->createMeshNode_( Helper::stringizeStringFormat( "ViewModel_%u", static_cast<uint32_t>(index) ),
                m_weaponResources[index], m_tileTexture, STRINGIZE_STRING_LOCAL( "Arena3D_ViewModel" ), m_viewCamera, post.get() );
            m_viewModels[index]->getTransformation()->setLocalScale( {0.48f, 0.48f, 0.48f} );
        }

        const mt::vec2f center( width * 0.5f, height * 0.5f );
        m_crosshairHorizontal = this->createHudQuad_( {18.f, 2.f}, {0.8f, 1.f, 0.95f, 0.9f}, &m_crosshairHorizontalSurface );
        m_crosshairVertical = this->createHudQuad_( {2.f, 18.f}, {0.8f, 1.f, 0.95f, 0.9f}, &m_crosshairVerticalSurface );
        m_crosshairHorizontal->getTransformation()->setLocalPosition( {center.x - 9.f, center.y - 1.f, 0.f} );
        m_crosshairVertical->getTransformation()->setLocalPosition( {center.x - 1.f, center.y - 9.f, 0.f} );
        SurfaceSolidColorPtr healthBackSurface;
        m_healthBack = this->createHudQuad_( {244.f, 18.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &healthBackSurface );
        m_healthFill = this->createHudQuad_( {240.f, 14.f}, {0.2f, 0.9f, 0.45f, 0.95f}, &m_healthFillSurface );
        m_healthBack->getTransformation()->setLocalPosition( {24.f, height - 42.f, 0.f} );
        m_healthFill->getTransformation()->setLocalPosition( {26.f, height - 40.f, 0.f} );
        SurfaceSolidColorPtr armorBackSurface;
        m_armorBack = this->createHudQuad_( {164.f, 12.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &armorBackSurface );
        m_armorFill = this->createHudQuad_( {160.f, 8.f}, {0.15f, 0.55f, 1.f, 0.95f}, &m_armorFillSurface );
        m_armorBack->getTransformation()->setLocalPosition( {24.f, height - 62.f, 0.f} );
        m_armorFill->getTransformation()->setLocalPosition( {26.f, height - 60.f, 0.f} );
        SurfaceSolidColorPtr speedBackSurface;
        m_speedBack = this->createHudQuad_( {204.f, 10.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &speedBackSurface );
        m_speedFill = this->createHudQuad_( {200.f, 6.f}, {0.12f, 0.9f, 0.85f, 0.9f}, &m_speedFillSurface );
        m_speedBack->getTransformation()->setLocalPosition( {width - 228.f, height - 42.f, 0.f} );
        m_speedFill->getTransformation()->setLocalPosition( {width - 226.f, height - 40.f, 0.f} );
        m_damageIndicator = this->createHudQuad_( {7.f, 30.f}, {1.f, 0.12f, 0.04f, 0.f}, &m_damageIndicatorSurface );
        m_damageIndicator->getTransformation()->setLocalPosition( {center.x, center.y - 120.f, 0.f} );

        this->syncPresentation_( 1.f );
        this->registerInput_();
        APPLICATION_SERVICE()->setCursorMode( false );
        m_timepipe = Helper::addTimepipe( [this]( const UpdateContext * context ){ this->update_( context ); }, MENGINE_DOCUMENT_FACTORABLE );
        LOGGER_MESSAGE( "Arena3D ready: generated Arena01, 100 Hz deterministic local server/client" );
        return true;
    }

    bool Arena3DSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        return true;
    }

    void Arena3DSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }

    void Arena3DSceneEventReceiver::registerButton_( EKeyCode _key, Arena3D::InputButton _button )
    {
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( _key, true, [this, _button]( const InputKeyEvent & event )
        {
            if( event.isRepeat == true ) return;
            Arena3D::CommandPayload payload;
            payload.button = _button;
            this->submit_( Arena3D::CommandType::ButtonDown, payload );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( _key, false, [this, _button]( const InputKeyEvent & event )
        {
            if( event.isRepeat == true ) return;
            Arena3D::CommandPayload payload;
            payload.button = _button;
            this->submit_( Arena3D::CommandType::ButtonUp, payload );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
    }

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
            m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( keys[index], true, [this, index]( const InputKeyEvent & event )
            {
                if( event.isRepeat == false ) this->selectWeapon_( static_cast<Arena3D::WeaponType>(index) );
            }, MENGINE_DOCUMENT_FACTORABLE ) );
        }

        m_inputHandlers.emplace_back( Helper::addGlobalMouseButtonHandler( MC_LBUTTON, true, [this]( const InputMouseButtonEvent & )
        { this->submit_( Arena3D::CommandType::FireDown ); }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalMouseButtonHandler( MC_LBUTTON, false, [this]( const InputMouseButtonEvent & )
        { this->submit_( Arena3D::CommandType::FireUp ); }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalMouseMoveHandler( [this]( const InputMouseMoveEvent & event )
        { this->onMouseMove_( event ); }, MENGINE_DOCUMENT_FACTORABLE ) );

        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_ESCAPE, true, [this]( const InputKeyEvent & event )
        {
            if( event.isRepeat == true ) return;
            const bool paused = m_session.server().state().paused;
            this->submit_( paused ? Arena3D::CommandType::Resume : Arena3D::CommandType::Pause );
            m_cursorCaptured = paused;
            APPLICATION_SERVICE()->setCursorMode( paused == false );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_F1, true, [this]( const InputKeyEvent & event )
        { if( event.isRepeat == false ) { m_debugHud = !m_debugHud; this->logDiagnostics_(); } }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_F2, true, [this]( const InputKeyEvent & event )
        { if( event.isRepeat == false ) { m_collisionDebug = !m_collisionDebug; LOGGER_MESSAGE( "Arena3D collision debug: %s", m_collisionDebug ? "on" : "off" ); } }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_F3, true, [this]( const InputKeyEvent & event )
        { if( event.isRepeat == false ) { m_turretDebug = !m_turretDebug; this->logDiagnostics_(); } }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_F4, true, [this]( const InputKeyEvent & event )
        {
            if( event.isRepeat == true ) return;
            m_noclip = !m_noclip;
            Arena3D::CommandPayload payload;
            payload.enabled = m_noclip;
            this->submit_( Arena3D::CommandType::Noclip, payload );
        }, MENGINE_DOCUMENT_FACTORABLE ) );
        m_inputHandlers.emplace_back( Helper::addGlobalKeyHandler( KC_F5, true, [this]( const InputKeyEvent & event )
        { if( event.isRepeat == false ) this->submit_( Arena3D::CommandType::Restart ); }, MENGINE_DOCUMENT_FACTORABLE ) );
    }

    void Arena3DSceneEventReceiver::unregisterInput_()
    {
        for( UniqueId handler : m_inputHandlers ) if( handler != INVALID_UNIQUE_ID ) Helper::removeGlobalHandler( handler );
        m_inputHandlers.clear();
    }

    void Arena3DSceneEventReceiver::submit_( Arena3D::CommandType _type, const Arena3D::CommandPayload & _payload )
    {
        m_session.submit( m_session.makeCommand( _type, m_sourceTimeMs, _payload ) );
    }

    void Arena3DSceneEventReceiver::selectWeapon_( Arena3D::WeaponType _weapon )
    {
        Arena3D::CommandPayload payload;
        payload.weapon = _weapon;
        this->submit_( Arena3D::CommandType::SelectWeapon, payload );
    }

    void Arena3DSceneEventReceiver::onMouseMove_( const InputMouseMoveEvent & _event )
    {
        if( m_cursorCaptured == false || _event.touchId != TC_TOUCH0 ) return;
        Arena3D::CommandPayload payload;
        const double sensitivity = 2.4;
        const Arena3D::Fixed yawRadians = Arena3D::Fixed::fromRaw( static_cast<int64_t>(_event.screenDelta.x * sensitivity * Arena3D::Fixed::Scale) );
        const Arena3D::Fixed pitchRadians = Arena3D::Fixed::fromRaw( static_cast<int64_t>(_event.screenDelta.y * sensitivity * Arena3D::Fixed::Scale) );
        payload.lookYaw = Arena3D::fixedSignedAngleFromRadians( yawRadians );
        payload.lookPitch = Arena3D::fixedSignedAngleFromRadians( pitchRadians );
        if( payload.lookYaw != 0 || payload.lookPitch != 0 )
            this->submit_( Arena3D::CommandType::LookDelta, payload );
    }

    void Arena3DSceneEventReceiver::update_( const UpdateContext * _context )
    {
        double elapsed = std::max( 0.0, std::min( 250.0, static_cast<double>(_context->time) ) );
        m_sourceTimeMs += static_cast<uint64_t>(elapsed);
        m_accumulatorMs += elapsed;
        while( m_accumulatorMs >= Arena3D::TickMilliseconds )
        {
            m_previousState = m_currentState;
            if( m_session.tick() == false )
            {
                LOGGER_ERROR( "Arena3D simulation fault/desync at tick %llu", static_cast<unsigned long long>(m_session.server().state().tick) );
                m_accumulatorMs = 0.0;
                break;
            }
            m_currentState = m_session.client().state();
            this->processEvents_();
            m_accumulatorMs -= Arena3D::TickMilliseconds;
        }
        m_hitFlash = std::max( 0.f, m_hitFlash - static_cast<float>(elapsed) * 0.004f );
        m_damageIndicatorTime = std::max( 0.f, m_damageIndicatorTime - static_cast<float>(elapsed) * 0.0015f );
        m_warningFlash = std::max( 0.f, m_warningFlash - static_cast<float>(elapsed) * 0.001f );
        Helper::staticNodeCast<PostProcessFx3D *>( m_postProcess.get() )->setHitFlash( {1.f, 0.12f, 0.04f}, m_hitFlash );
        this->syncPresentation_( static_cast<float>(m_accumulatorMs / Arena3D::TickMilliseconds) );
    }

    void Arena3DSceneEventReceiver::processEvents_()
    {
        const uint64_t matchId = m_session.server().state().matchId;
        if( m_presentedMatchId != matchId )
        {
            m_presentedMatchId = matchId;
            m_lastPresentedEventId = UINT64_MAX;
        }
        for( const Arena3D::ServerEvent & event : m_session.server().events() )
        {
            if( m_lastPresentedEventId != UINT64_MAX && event.id <= m_lastPresentedEventId ) continue;
            m_lastPresentedEventId = event.id;
            if( event.type == Arena3D::EventType::Shot )
            {
                static const size_t weaponSounds[] = {0, 1, 3, 4, 5, 6};
                this->playSound_( weaponSounds[weaponIndex( event.weapon )] );
            }
            else if( event.type == Arena3D::EventType::Hit || event.type == Arena3D::EventType::Explosion ) this->playSound_( 7 );
            else if( event.type == Arena3D::EventType::JumpPad ) this->playSound_( 9 );
            else if( event.type == Arena3D::EventType::TurretWarning ) this->playSound_( 12 );
            else if( event.type == Arena3D::EventType::PlayerDeath ) this->playSound_( 11 );
            else if( event.type == Arena3D::EventType::PlayerRespawned ) this->playSound_( 8 );
            else if( event.type == Arena3D::EventType::Pickup ) this->playSound_( 8 );
            if( event.type == Arena3D::EventType::TurretWarning ) m_warningFlash = 1.f;
            if( event.type == Arena3D::EventType::Damage && event.targetId == m_currentState.player.id )
            {
                m_hitFlash = 0.65f;
                m_damageIndicatorTime = 1.f;
                const mt::vec3f delta = toVec3( event.position - m_currentState.player.position );
                const float relative = wrapRadians( StdMath::atan2f( delta.x, delta.z ) - angleToRadians( m_currentState.player.yaw ) );
                const Resolution & resolution = APPLICATION_SERVICE()->getContentResolution();
                const float centerX = resolution.getWidthF() * 0.5f;
                const float centerY = resolution.getHeightF() * 0.5f;
                m_damageIndicator->getTransformation()->setLocalPosition( {centerX + StdMath::sinf( relative ) * 120.f,
                    centerY - StdMath::cosf( relative ) * 120.f, 0.f} );
                m_damageIndicator->getTransformation()->setLocalOrientationZ( -relative );
            }
            if( event.type == Arena3D::EventType::PlayerDeath ) m_hitFlash = 1.f;
        }
    }

    void Arena3DSceneEventReceiver::playSound_( size_t _index )
    {
        if( _index >= m_sounds.size() || m_sounds[_index] == nullptr ) return;
        AnimationInterface * animation = m_sounds[_index]->getAnimation();
        if( animation->isPlay() == true ) animation->stop();
        animation->play( 0.f );
    }

    void Arena3DSceneEventReceiver::syncPresentation_( float _alpha )
    {
        this->syncCamera_( m_currentState.player, _alpha );
        this->syncArenaChunks_();
        this->syncTurrets_();
        this->syncProjectiles_( _alpha );
        this->syncPickups_();
        this->syncViewModels_();
        this->syncHud_();
        m_drawCalls = m_visibleChunks + static_cast<uint32_t>(Arena3D::MaximumTurrets * 2u) +
            static_cast<uint32_t>(Arena3D::MaximumPickups) + m_visibleProjectiles + 1u;
    }

    void Arena3DSceneEventReceiver::syncArenaChunks_()
    {
        static const mt::vec3f centers[] = {
            {-16.f, 0.f, -16.f}, {16.f, 0.f, -16.f}, {-16.f, 0.f, 16.f}, {16.f, 0.f, 16.f}};
        const mt::vec3f player = toVec3( m_currentState.player.position );
        const float yaw = angleToRadians( m_currentState.player.yaw );
        const float forwardX = StdMath::sinf( yaw );
        const float forwardZ = StdMath::cosf( yaw );
        constexpr float chunkRadius = 23.f;
        constexpr float horizontalHalfFovCosine = 0.68f;

        m_visibleChunks = 0;
        for( size_t index = 0; index != m_arenaChunkNodes.size(); ++index )
        {
            const float dx = centers[index].x - player.x;
            const float dz = centers[index].z - player.z;
            const float distance = StdMath::sqrtf( dx * dx + dz * dz );
            const float forwardDistance = dx * forwardX + dz * forwardZ;
            const bool visible = distance <= chunkRadius ||
                forwardDistance + chunkRadius >= distance * horizontalHalfFovCosine;
            m_arenaChunkNodes[index]->getTransformation()->setLocalPosition(
                visible ? mt::vec3f( 0.f, 0.f, 0.f ) : mt::vec3f( 0.f, -10000.f, 0.f ) );
            m_visibleChunks += visible ? 1u : 0u;
        }
    }

    void Arena3DSceneEventReceiver::syncCamera_( const Arena3D::PlayerState & _player, float _alpha )
    {
        const mt::vec3f position = interpolate( m_previousState.player.position, _player.position, _alpha );
        const float yaw = angleToRadians( _player.yaw );
        const float pitch = angleToRadians( _player.pitch );
        const float cp = StdMath::cosf( pitch );
        const mt::vec3f direction( StdMath::sinf( yaw ) * cp, StdMath::sinf( pitch ), StdMath::cosf( yaw ) * cp );
        m_worldCamera->setCameraPosition( position + mt::vec3f( 0.f, _player.crouched ? 1.0f : 1.6f, 0.f ) );
        m_worldCamera->setCameraDirection( direction );
        m_worldCamera->setCameraUp( {0.f, -1.f, 0.f} );
    }

    void Arena3DSceneEventReceiver::syncTurrets_()
    {
        for( size_t index = 0; index != Arena3D::MaximumTurrets; ++index )
        {
            const Arena3D::TurretStateData & turret = m_currentState.turrets[index];
            const bool visible = turret.state != Arena3D::TurretState::Destroyed && turret.state != Arena3D::TurretState::Respawning;
            m_turretBaseNodes[index]->getTransformation()->setLocalPosition( visible ? toVec3( turret.position ) : mt::vec3f( 0.f, -10000.f, 0.f ) );
            float yaw = angleToRadians( turret.yaw );
            float pitch = angleToRadians( turret.pitch );
            if( turret.state != Arena3D::TurretState::Searching && visible == true )
            {
                const mt::vec3f delta = toVec3( m_currentState.player.position - turret.position );
                yaw = StdMath::atan2f( delta.x, delta.z );
                pitch = -StdMath::atan2f( delta.y, StdMath::sqrtf( delta.x * delta.x + delta.z * delta.z ) );
            }
            m_turretBaseNodes[index]->getTransformation()->setLocalOrientationY( yaw );
            m_turretBarrelNodes[index]->getTransformation()->setLocalOrientationX( pitch );
        }
    }

    void Arena3DSceneEventReceiver::syncProjectiles_( float _alpha )
    {
        std::array<uint32_t, static_cast<size_t>(Arena3D::WeaponType::Count)> active{};
        for( size_t slot = 0; slot != Arena3D::MaximumProjectiles; ++slot )
        {
            const Arena3D::ProjectileState & projectile = m_currentState.projectiles[slot];
            if( projectile.active == false ) continue;
            const size_t weapon = weaponIndex( projectile.weapon );
            Vector<NodePtr> & pool = m_projectilePools[weapon];
            const uint32_t poolIndex = active[weapon]++;
            if( poolIndex == pool.size() )
            {
                NodePtr node = this->createMeshNode_( Helper::stringizeStringFormat( "Projectile_%u_%u", static_cast<uint32_t>(weapon), poolIndex ),
                    m_projectileResources[weapon], m_emissiveTexture, STRINGIZE_STRING_LOCAL( "Arena3D_Emissive" ),
                    m_worldCamera, m_postProcess.get() );
                pool.emplace_back( node );
            }
            const Arena3D::ProjectileState & previous = m_previousState.projectiles[slot];
            const mt::vec3f position = previous.active && previous.id == projectile.id
                ? interpolate( previous.position, projectile.position, _alpha )
                : toVec3( projectile.position );
            pool[poolIndex]->getTransformation()->setLocalPosition( position );
        }
        m_visibleProjectiles = 0;
        for( size_t weapon = 0; weapon != active.size(); ++weapon )
        {
            this->parkUnused_( m_projectilePools[weapon], active[weapon] );
            m_visibleProjectiles += active[weapon];
        }
    }

    void Arena3DSceneEventReceiver::syncViewModels_()
    {
        const size_t selected = weaponIndex( m_currentState.player.selectedWeapon );
        for( size_t index = 0; index != m_viewModels.size(); ++index )
        {
            m_viewModels[index]->getTransformation()->setLocalPosition( index == selected
                ? mt::vec3f( -0.55f, 0.42f, 1.65f )
                : mt::vec3f( 0.f, -10000.f, 0.f ) );
        }
    }

    void Arena3DSceneEventReceiver::syncPickups_()
    {
        for( size_t index = 0; index != Arena3D::MaximumPickups; ++index )
        {
            const Arena3D::PickupStateData & pickup = m_currentState.pickups[index];
            m_pickupNodes[index]->getTransformation()->setLocalPosition( pickup.active == true
                ? toVec3( pickup.position )
                : mt::vec3f( 0.f, -10000.f, 0.f ) );
            m_pickupNodes[index]->getTransformation()->setLocalOrientationY( static_cast<float>(m_currentState.tick % 628u) * 0.01f );
        }
    }

    void Arena3DSceneEventReceiver::syncHud_()
    {
        const Arena3D::PlayerState & player = m_currentState.player;
        const float health = std::max( 0.f, std::min( 1.f, static_cast<float>(player.health) / 100.f ) );
        const float armor = std::max( 0.f, std::min( 1.f, static_cast<float>(player.armor) / 100.f ) );
        const Arena3D::Fixed horizontalSpeedSquared = player.velocity.x * player.velocity.x + player.velocity.z * player.velocity.z;
        const float speed = std::max( 0.f, std::min( 1.f, static_cast<float>(Arena3D::fixedSqrt( horizontalSpeedSquared ).toDouble()) / 12.f ) );
        m_healthFillSurface->setSolidSize( {240.f * health, 14.f} );
        m_armorFillSurface->setSolidSize( {160.f * armor, 8.f} );
        m_speedFillSurface->setSolidSize( {200.f * speed, 6.f} );
        m_damageIndicatorSurface->setSolidColor( {1.f, 0.12f, 0.04f, m_damageIndicatorTime} );
        const Color crosshair = m_warningFlash > 0.f ? Color( 1.f, 0.15f, 0.75f, 1.f ) :
            (player.fireHeld ? Color( 1.f, 0.32f, 0.14f, 1.f ) : Color( 0.8f, 1.f, 0.95f, 0.9f ));
        m_crosshairHorizontalSurface->setSolidColor( crosshair );
        m_crosshairVerticalSurface->setSolidColor( crosshair );
    }

    void Arena3DSceneEventReceiver::parkUnused_( Vector<NodePtr> & _pool, uint32_t _activeCount )
    {
        for( uint32_t index = _activeCount; index != _pool.size(); ++index )
            _pool[index]->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
    }

    void Arena3DSceneEventReceiver::logDiagnostics_() const
    {
        const Arena3D::StateChecksum checksum = m_session.server().checksum();
        LOGGER_MESSAGE( "Arena3D tick=%llu crc=%08X hp=%d speed=%.3f draws=%u chunks=%u projectiles=%u pools=%u reconciliations=%llu",
            static_cast<unsigned long long>(m_session.server().state().tick), checksum.full,
            m_currentState.player.health, static_cast<float>(Arena3D::fixedLength( m_currentState.player.velocity ).toDouble()),
            m_drawCalls, m_visibleChunks, m_visibleProjectiles,
            static_cast<uint32_t>(m_projectilePools[0].size() + m_projectilePools[1].size() + m_projectilePools[2].size() +
                m_projectilePools[3].size() + m_projectilePools[4].size() + m_projectilePools[5].size()),
            static_cast<unsigned long long>(m_session.reconciliationCount()) );
        if( m_turretDebug == true )
        {
            for( const Arena3D::TurretStateData & turret : m_currentState.turrets )
            {
                LOGGER_MESSAGE( "Arena3D turret=%u state=%u hp=%d cooldown=%u", turret.id,
                    static_cast<uint32_t>(turret.state), turret.health, turret.cooldownTicks );
            }
        }
    }
}
