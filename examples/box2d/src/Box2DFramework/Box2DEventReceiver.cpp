#include "Box2DEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"
#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Engine/Engine.h"
#include "Engine/HotSpotGlobal.h"
#include "Engine/HotSpotCircle.h"

#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeCircle.h"
#include "Kernel/Logger.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Vector.h"
#include "Kernel/NodeCast.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/RenderCameraHelper.h"
#include "Kernel/NodeRenderHierarchy.h"

#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Box2DEventReceiver::Box2DEventReceiver()
        : m_scene( nullptr )        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DEventReceiver::~Box2DEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
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
    bool Box2DEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        // create node for box2d objects
        NodePtr node = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Interender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( node );

        m_boxNode = node;

        m_scene->addChild( m_boxNode );

        // create box2d objects
        mt::vec2f gravity( 0.f, 0.f );

        Box2DWorldInterfacePtr world = BOX2D_SERVICE()
            ->createWorld( gravity, 1.f, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( world );

        m_world = world;

        float timeStep = 1000.f / 60.0f;
        uint32_t subStepCount = 4;

        m_world->setTimeStep( timeStep, subStepCount );

        // create box with dynamic body
        for( uint32_t index = 0; index != 100; ++index )
        {
            float x = (index % 10) * 50.f + 250.f;
            float y = (index / 10) * 50.f + 150.f;

            Box2DBodyInterfacePtr body = m_world->createBody(
                false
                , {x, y}, 0.f, 5.f, 0.f
                , true, false, true
                , MENGINE_DOCUMENT_FACTORABLE
            );

            float shapeRaius = 20.f;

            body->addShapeCircle( shapeRaius, {0.f, 0.f}, 1.f, 0.99f, 0.1f, false, 0x00000002, 0xFFFF, 0 );

            // vectorizer
            NodePtr graphicsNode = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Graphics" ), MENGINE_DOCUMENT_FACTORABLE );

            GraphicsInterface * graphics = graphicsNode->getUnknown();

            Color colorLine = Helper::makeColor8( 255, 0, 0, 255 );
            Color colorFill = Helper::makeColor8( 0, 255, 0, 255 );

            graphics->setLineColor( colorLine );
            graphics->setLineWidth( 1.f );

            graphics->drawCircle( {0.f, 0.f}, shapeRaius );

            // create box
            NodePtr phisicalPlaceholder = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "PhysicalPlaceholder" ), MENGINE_DOCUMENT_FACTORABLE );

            UnknownPhysicalPlaceholderInterface * unknownPhisicalPlaceholder = phisicalPlaceholder->getUnknown();

            unknownPhisicalPlaceholder->setBox2DBody( body );

            phisicalPlaceholder->addChild( graphicsNode );

            m_boxNode->addChild( phisicalPlaceholder );
        }

        m_hero = Helper::makeFactorableUnique<Hero>( MENGINE_DOCUMENT_FACTORABLE );

        jpp::object data;

        m_hero->initialize( m_world, m_boxNode, {0.f, 0.f}, data );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DEventReceiver::clickExplosion( const InputMouseButtonEvent & _event )
    {
        mt::vec2f wp;
        PLAYER_SERVICE()
            ->calcGlobalMouseWorldPosition( {_event.x, _event.y}, &wp );

        float radius = 100.f;

        float sqrradius = radius * radius;

        Box2DBodyInterface * bodies[1024] = {nullptr};
        uint32_t found = m_world->overlapCircle( wp, radius, 1U, ~0U, bodies, 1024 );

        for( uint32_t index = 0; index != found; ++index )
        {
            Box2DBodyInterface * body = bodies[index];

            mt::vec2f body_position = body->getBodyPosition();

            mt::vec2f impulse_vector = body_position - wp;
            mt::vec2f impulse_direction = mt::norm_v2( impulse_vector );
            float sqrlength = mt::sqrlength_v2( impulse_vector );

            if( sqrlength > sqrradius )
            {
                continue;
            }

            float force = 1.f - sqrlength / sqrradius;

            mt::vec2f impulse = impulse_direction * force * 1000000.f;
            mt::vec2f point = body_position;

            body->applyImpulse( impulse, point );
        }
    }
    //////////////////////////////////////////////////////////////////////////
};