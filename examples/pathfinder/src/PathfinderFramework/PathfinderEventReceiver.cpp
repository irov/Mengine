#include "PathfinderEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Frameworks/SimpleFramework/SimpleSprite.h"
#include "Frameworks/SimpleFramework/SimpleRandomizer.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderEventReceiver::PathfinderEventReceiver()
        : m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderEventReceiver::~PathfinderEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        // setup box2d node position at screen center
        //const Resolution & resolution = APPLICATION_SERVICE()
        //    ->getContentResolution();

        //float width = resolution.getWidthF();
        //float height = resolution.getHeightF();

        //m_node->setLocalPosition( { width / 2.f, height / 4.f, 0.f } );
        //m_node->setLocalScale( {256.f, 256.f, 1.f} );

        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "Assets" ) );

        uint32_t width = 20;
        uint32_t height = 15;

        float size = 50.f;

        for( uint32_t i = 0; i != width; ++i )
        {
            for( uint32_t j = 0; j != height; ++j )
            {
                ShapeQuadSizePtr grass = Helper::createSimpleSprite( fileGroup, STRINGIZE_FILEPATH_LOCAL( "sprite/grass.png" ), mt::vec2f( 100.f, 100.f ), mt::vec2f( size, size ), MENGINE_DOCUMENT_FACTORABLE );

                TransformationInterface * transformation = grass->getTransformation();

                transformation->setLocalPosition( mt::vec3f( i * size, j * size, 0.f ) );

                node->addChild( grass );
            }
        }

        for( uint32_t i = 0; i != width; ++i )
        {
            for( uint32_t j = 0; j != height; ++j )
            {
                uint32_t r = Helper::getSimpleRandomizerRandom( 4 );

                if( r != 0 )
                {
                    continue;
                }

                ShapeQuadSizePtr stone = Helper::createSimpleSprite( fileGroup, STRINGIZE_FILEPATH_LOCAL( "sprite/stone.png" ), mt::vec2f( 100.f, 100.f ), mt::vec2f( size, size ), MENGINE_DOCUMENT_FACTORABLE );

                TransformationInterface * transformation = stone->getTransformation();

                transformation->setLocalPosition( mt::vec3f( i * size, j * size, 0.f ) );

                node->addChild( stone );
            }
        }

        m_scene->addChild( node );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
};