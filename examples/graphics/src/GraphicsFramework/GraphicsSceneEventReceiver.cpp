#include "GraphicsSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NodeCast.h"

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

        NodePtr graphicsNode = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FACTORABLE );

        GraphicsInterface * graphics = graphicsNode->getUnknown();

        graphics->setLineWidth( 10.f );
        graphics->setLineSoft( 4.f );

        graphics->beginFill();
        graphics->setLineColor( {1.f, 0.25f, 0.f, 1.f} );
        graphics->drawCircle( {200.f, 300.f}, 50.f );

        const VectorGraphicsGradientStops sky = {
            {0.f, Color( 79.f / 255.f, 183.f / 255.f, 1.f, 1.f )},
            {1.f, Color( 168.f / 255.f, 228.f / 255.f, 1.f, 1.f )}
        };

        if( graphics->setLinearGradient( {400.f, 300.f}, {400.f, 400.f}, sky ) == false )
        {
            return false;
        }

        graphics->drawRect( {400.f, 300.f}, 100.f, 100.f );

        const VectorGraphicsGradientStops radial = {
            {0.f, Color( 1.f, 0.9f, 0.5f, 1.f )},
            {0.65f, Color( 0.9f, 0.25f, 0.5f, 0.85f )},
            {1.f, Color( 0.25f, 0.3f, 1.f, 0.6f )}
        };

        graphics->pushState();
        if( graphics->setRadialGradient( {630.f, 325.f}, 60.f, radial ) == false )
        {
            graphics->popState();

            return false;
        }

        graphics->drawRoundedRect( {600.f, 300.f}, 100.f, 100.f, 25.f );
        graphics->popState();
        graphics->drawEllipse( {800.f, 350.f}, 50.f, 35.f );
        graphics->clearFillGradient();
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
