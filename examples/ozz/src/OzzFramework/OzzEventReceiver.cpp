#include "OzzEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Engine/Engine.h"
#include "Engine/HotSpotGlobal.h"
#include "Engine/HotSpotCircle.h"

#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeCircle.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Vector.h"
#include "Kernel/GlobalInputHandlerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OzzEventReceiver::OzzEventReceiver()
        : m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OzzEventReceiver::~OzzEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        if( m_node != nullptr )
        {
            m_node->removeFromParent();
            m_node = nullptr;
        }

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        // create node for box2d objects
        NodePtr node = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        m_node = node;

        m_scene->addChild( m_node );

        // setup box2d node position at screen center
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float width = resolution.getWidthF();
        float height = resolution.getHeightF();

        TransformationInterface * nodeTransformation = m_node->getTransformation();
        nodeTransformation->setLocalPosition( {width / 2.f, height / 4.f, 0.f} );
        nodeTransformation->setLocalScale( {256.f, 256.f, 1.f} );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "Assets" ) );

        ResourcePtr resourceOzzSkeleton = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzSkeleton" ), MENGINE_DOCUMENT_FUNCTION );

        ContentInterfacePtr resourceOzzSkeletonContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        resourceOzzSkeletonContent->setFileGroup( fileGroup );
        resourceOzzSkeletonContent->setFilePath( STRINGIZE_FILEPATH_LOCAL( "ozz/pab_skeleton.ozz" ) );

        resourceOzzSkeleton->setContent( resourceOzzSkeletonContent );

        if( resourceOzzSkeleton->initialize() == false )
        {
            return false;
        }

        ResourcePtr resourceOzzMesh = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzMesh" ), MENGINE_DOCUMENT_FUNCTION );

        ContentInterfacePtr resourceOzzMeshContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        resourceOzzMeshContent->setFileGroup( fileGroup );
        resourceOzzMeshContent->setFilePath( STRINGIZE_FILEPATH_LOCAL( "ozz/arnaud_mesh.ozz" ) );

        resourceOzzMesh->setContent( resourceOzzMeshContent );

        if( resourceOzzMesh->initialize() == false )
        {
            return false;
        }

        NodePtr nodeOzzAnimation = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeOzzAnimation" ), MENGINE_DOCUMENT_FUNCTION );

        UnknownNodeOzzAnimationInterface * unknownNodeOzzAnimation = nodeOzzAnimation->getUnknown();

        ResourceImagePtr resourceImageDefault = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceImageDefault" ), MENGINE_DOCUMENT_FUNCTION );

        resourceImageDefault->setMaxSize( {256.f, 256.f} );

        ContentInterfacePtr resourceImageDefaultContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        resourceImageDefaultContent->setCodecType( STRINGIZE_STRING_LOCAL( "pngImage" ) );
        resourceImageDefaultContent->setFileGroup( fileGroup );
        resourceImageDefaultContent->setFilePath( STRINGIZE_FILEPATH_LOCAL( "ozz/texture.png" ) );

        resourceImageDefault->setContent( resourceImageDefaultContent );

        if( resourceImageDefault->initialize() == false )
        {
            return false;
        }

        unknownNodeOzzAnimation->setResourceOzzImage( resourceImageDefault );
        unknownNodeOzzAnimation->setResourceOzzSkeleton( resourceOzzSkeleton );
        unknownNodeOzzAnimation->setResourceOzzMesh( resourceOzzMesh );

        SamplerOzzAnimationInterfacePtr samplerOzzAnimation_walk = this->createOzzSampler( fileGroup, STRINGIZE_FILEPATH_LOCAL( "ozz/pab_walk.ozz" ), resourceOzzSkeleton, 0.f );
        SamplerOzzAnimationInterfacePtr samplerOzzAnimation_jog = this->createOzzSampler( fileGroup, STRINGIZE_FILEPATH_LOCAL( "ozz/pab_jog.ozz" ), resourceOzzSkeleton, 0.f );

        unknownNodeOzzAnimation->addOzzAnimationSampler( samplerOzzAnimation_walk );
        unknownNodeOzzAnimation->addOzzAnimationSampler( samplerOzzAnimation_jog );

        m_node->addChild( nodeOzzAnimation );

        Helper::addGlobalKeyHandler( KC_1, true, [samplerOzzAnimation_walk, samplerOzzAnimation_jog]( const InputKeyEvent & )
        {
            samplerOzzAnimation_walk->setWeight( 1.f );
            samplerOzzAnimation_jog->setWeight( 0.f );
        }, MENGINE_DOCUMENT_FACTORABLE );

        Helper::addGlobalKeyHandler( KC_2, true, [samplerOzzAnimation_walk, samplerOzzAnimation_jog]( const InputKeyEvent & )
        {
            samplerOzzAnimation_walk->setWeight( 0.75f );
            samplerOzzAnimation_jog->setWeight( 0.25f );
        }, MENGINE_DOCUMENT_FACTORABLE );

        Helper::addGlobalKeyHandler( KC_3, true, [samplerOzzAnimation_walk, samplerOzzAnimation_jog]( const InputKeyEvent & )
        {
            samplerOzzAnimation_walk->setWeight( 0.25f );
            samplerOzzAnimation_jog->setWeight( 0.75f );
        }, MENGINE_DOCUMENT_FACTORABLE );

        Helper::addGlobalKeyHandler( KC_4, true, [samplerOzzAnimation_walk, samplerOzzAnimation_jog]( const InputKeyEvent & )
        {
            samplerOzzAnimation_walk->setWeight( 0.f );
            samplerOzzAnimation_jog->setWeight( 1.f );
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    SamplerOzzAnimationInterfacePtr OzzEventReceiver::createOzzSampler( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ResourcePtr & _resourceOzzSkeleton, float _weight )
    {
        ResourcePtr resourceOzzAnimation = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceOzzAnimation" ), MENGINE_DOCUMENT_FUNCTION );

        ContentInterfacePtr resourceOzzAnimationContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        resourceOzzAnimationContent->setFileGroup( _fileGroup );
        resourceOzzAnimationContent->setFilePath( _filePath );

        resourceOzzAnimation->setContent( resourceOzzAnimationContent );

        if( resourceOzzAnimation->initialize() == false )
        {
            return nullptr;
        }

        SamplerOzzAnimationInterfacePtr samplerOzzAnimation = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Sampler" ), STRINGIZE_STRING_LOCAL( "SamplerOzzAnimation" ), MENGINE_DOCUMENT_FUNCTION );

        samplerOzzAnimation->setResourceOzzSkeleton( _resourceOzzSkeleton );
        samplerOzzAnimation->setResourceOzzAnimation( resourceOzzAnimation );
        samplerOzzAnimation->setWeight( _weight );

        return samplerOzzAnimation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool OzzEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void OzzEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
};