#include "SimpleBox2DEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Engine/Engine.h"
#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/ShapeCircle.h"
#include "Engine/HotSpotGlobal.h"
#include "Engine/HotSpotCircle.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DEventReceiver::SimpleBox2DEventReceiver()
        : m_scene( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DEventReceiver::~SimpleBox2DEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        if( m_boxNode != nullptr )
        {
            m_boxNode->removeFromParent();
            m_boxNode = nullptr;
        }

        m_scene = nullptr;

        m_world = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        // create node for box2d objects
        NodePtr node = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Node" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( node, false );

        m_boxNode = node;

        m_scene->addChild( m_boxNode );

        // setup box2d node position at screen center
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float width = resolution.getWidthF();
        float height = resolution.getHeightF();

        m_boxNode->setLocalPosition( { width / 2.f, height / 2.f, 0.f } );

        // create box2d objects
        mt::vec2f gravity( 0.f, 10.f );

        Box2DWorldInterfacePtr world = BOX2D_SERVICE()
            ->createWorld( gravity, 1.f, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( world, false );

        m_world = world;

        float timeStep = 1.0f / 60.0f;
        uint32_t velocityIterations = 8;
        uint32_t positionIterations = 3;

        m_world->setTimeStep( timeStep, velocityIterations, positionIterations );

        // create box with dynamic body
        if( true )
        {
            Box2DBodyInterfacePtr body = m_world->createBody(
                false
                , { -100.f, -200.f }, 0.f, 0.f, 0.f
                , true, false, false
                , MENGINE_DOCUMENT_FUNCTION
            );

            mt::vec2f shapeSize( 50.f, 50.f );

            body->addShapeBox(
                shapeSize.x * 0.5f, shapeSize.y * 0.5f, { 0.f, 0.f }, 0.f
                , 1.f, 0.2f, 0.f, false
                , 0xFFFF, 0x0001, 0
            );

            // transforms
            body->setLinearVelocity( { 20.f, 0.f } );
            //body->setAngularVelocity( -90.f * mt::constant::deg2rad );

            // vectorizer
            NodePtr graphicsNode = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FUNCTION );

            graphicsNode->setName( STRINGIZE_STRING_LOCAL( "Vectorizator_DynamicBody" ) );

            GraphicsInterface * graphics = graphicsNode->getUnknown();

            Color colorLine( Helper::makeRGBA8( 255, 0, 0, 255 ) );
            Color colorFill( Helper::makeRGBA8( 0, 255, 0, 255 ) );

            graphics->setLineColor( colorLine );
            graphics->setLineWidth( 1.f );

            mt::vec2f size( shapeSize );
            mt::vec2f halfSize = size * 0.5;

            graphics->drawRect( {-halfSize.x, -halfSize.y}, size.x, size.y );

            graphics->pushState();
            graphics->setLineColor( colorFill );
            graphics->beginFill();
            graphics->drawRect( {-halfSize.x * 0.5f, -halfSize.y * 0.5f}, size.x * 0.5f, size.y * 0.5f );
            graphics->endFill();
            graphics->popState();

            m_boxNode->addChild( graphicsNode );

            // create box
            NodePtr phisicalPlaceholder = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), MENGINE_DOCUMENT_FACTORABLE );

            UnknownPhysicalPlaceholderInterface * unknownPhisicalPlaceholder = phisicalPlaceholder->getUnknown();

            unknownPhisicalPlaceholder->setBox2DBody( body );

            phisicalPlaceholder->addChild( graphicsNode );

            m_boxNode->addChild( phisicalPlaceholder );
        }

        // create box with static body
        if( true )
        {
            // static body
            Box2DBodyInterfacePtr body = m_world->createBody(
                true
                , { 0.f, 0.f }, 0.f, 0.f, 0.f
                , true, false, false
                , MENGINE_DOCUMENT_FUNCTION
            );

            mt::vec2f shapeSize( 50.f, 50.f );

            body->addShapeBox(
                shapeSize.x * 0.5f, shapeSize.y * 0.5f, { 0.f, 0.f }, 0.f
                , 1.f, 0.2f, 0.f, false
                , 0xFFFF, 0x0001, 0
            );

            // vectorizer
            NodePtr graphicsNode = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FUNCTION );

            graphicsNode->setName( STRINGIZE_STRING_LOCAL( "Vectorizator_StaticBody" ) );

            GraphicsInterface * graphics = graphicsNode->getUnknown();

            Color colorLine( Helper::makeRGBA8( 255, 0, 0, 255 ) );
            Color colorFill( Helper::makeRGBA8( 0, 255, 255, 255 ) );

            graphics->setLineColor( colorLine );
            graphics->setLineWidth( 1.f );

            mt::vec2f size( shapeSize );
            mt::vec2f halfSize = size * 0.5;

            graphics->drawRect( {-halfSize.x, -halfSize.y}, size.x, size.y );

            graphics->pushState();
            graphics->setLineColor( colorFill );
            graphics->beginFill();
            graphics->drawRect( { -halfSize.x * 0.5f, -halfSize.y * 0.5f}, size.x * 0.5f, size.y * 0.5f );
            graphics->endFill();
            graphics->popState();

            NodePtr phisicalPlaceholder = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), MENGINE_DOCUMENT_FACTORABLE );

            UnknownPhysicalPlaceholderInterface * unknownPhisicalPlaceholder = phisicalPlaceholder->getUnknown();

            unknownPhisicalPlaceholder->setBox2DBody( body );

            phisicalPlaceholder->addChild( graphicsNode );

            m_boxNode->addChild( phisicalPlaceholder );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        // task chain
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addGlobalMouseButton( source, MC_LBUTTON, true, nullptr );
        Cook::addPrint( source, "Click" );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        if( m_chain != nullptr )
        {
            m_chain->cancel();

            m_chain = nullptr;
        }
    }
};