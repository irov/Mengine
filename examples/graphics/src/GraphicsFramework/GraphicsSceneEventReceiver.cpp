#include "GraphicsSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsSceneEventReceiver::GraphicsSceneEventReceiver()
        : m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsSceneEventReceiver::~GraphicsSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        NodePtr graphicsNode = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FACTORABLE );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        graphics->setLineWidth( 10.f );
        graphics->setLineSoft( 4.f );

        graphics->beginFill();
        graphics->setLineColor( {1.f, 0.25f, 0.f, 1.f} );
        graphics->drawCircle( {200.f, 300.f}, 50.f );

        graphics->setLineColor( {0.5f, 0.25f, 1.f, 1.f} );
        graphics->drawRect( {400.f, 300.f}, 100.f, 100.f );

        graphics->setLineColor( {0.25f, 1.f, 0.75f, 1.f} );
        graphics->drawRoundedRect( {600.f, 300.f}, 100.f, 100.f, 25.f );
        graphics->endFill();

        m_scene->addChild( graphicsNode );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        
        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

    }
};