#include "SimpleBox2DEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/ApplicationInterface.h"

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

#include "Tasks/TaskGlobalMouseButton.h"
#include "Tasks/TaskPrint.h"

#include "Config/Vector.h"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SimpleBox2DEventReceiver::SimpleBox2DEventReceiver()
        : m_scene( nullptr )
        , m_timepipeId( 0 )
        , m_worldOffset( 0.f, 0.f, 0.f )
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

        if( m_vectorizator != nullptr )
        {
            m_vectorizator->removeFromParent();
            m_vectorizator->release();

            m_vectorizator = nullptr;
        }

        m_scene = nullptr;

        m_body = nullptr;

        m_world = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        mt::vec2f gravity( 0.0f, 10.0f );

        Box2DWorldInterfacePtr world = BOX2D_SERVICE()
            ->createWorld( gravity, 100.f, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( world, false );

        m_world = world;

        float timeStep = 1.0f / 60.0f;
        uint32_t velocityIterations = 6;
        uint32_t positionIterations = 2;

        m_world->setTimeStep( timeStep, velocityIterations, positionIterations );

        m_body = m_world->createBody(
            false
            , { 0.f, 20.f }, 0.f, 0.f, 0.f
            , true, false, false
            , MENGINE_DOCUMENT_FUNCTION
        );

        m_body->addShapeBox(
            1.f, 1.f, { 0.f, 0.f }, 0.f
            , 1.f, 0.2f, 0.f, false
            , 0xFFFF, 0x0001, 0
        );

        // transforms
        //m_body->SetTransform( b2Vec2( 10.f, 20.f ), 45.f * DEGTORAD );
        m_body->setLinearVelocity( { -5.f, 5.f } );
        m_body->setAngularVelocity( -90.f * DEGTORAD );

        // vectorizer

        VectorizatorPtr vectorizator = Helper::generateVectorizator( MENGINE_DOCUMENT_FUNCTION );

        vectorizator->setName( STRINGIZE_STRING_LOCAL( "Vectorizator_Tire" ) );

        Color colorLine( Helper::makeARGB8( 255, 0, 0, 255 ) );
        Color colorFill( Helper::makeARGB8( 0, 255, 0, 255 ) );

        vectorizator->setLineColor( colorLine );
        vectorizator->setLineWidth( 5.f );

        mt::vec2f size( 100.f, 100.f );
        mt::vec2f halfSize = size * 0.5;

        vectorizator->beginFill( colorFill );
        vectorizator->drawRect( { -halfSize.x, -halfSize.y }, size.x, size.y );
        vectorizator->endFill();

        vectorizator->drawCircle( { 0.f, 0.f }, 5.f );

        m_vectorizator = vectorizator;

        m_scene->addChild( vectorizator );

        // setup world center
        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float width = resolution.getWidthF();
        float height = resolution.getHeightF();

        m_worldOffset.x = width / 2.f;
        m_worldOffset.y = height / 2.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SimpleBox2DEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        GOAP::SourcePtr source = GOAP::Helper::makeSource();

        source->addTask<TaskGlobalMouseButton>( MC_LBUTTON, true, nullptr );
        source->addTask<TaskPrint>( "Click" );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );

        chain->run();

        m_chain = chain;

        // run box2d

        uint32_t timepipeId = TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ) );

        if( timepipeId == 0 )
        {
            return false;
        }

        m_timepipeId = timepipeId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        if( m_timepipeId != 0 )
        {
            TIMEPIPE_SERVICE()
                ->removeTimepipe( m_timepipeId );

            m_timepipeId = 0;
        }

        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SimpleBox2DEventReceiver::onTimepipe( const UpdateContext * _context )
    {
        MENGINE_UNUSED( _context );

        if( m_vectorizator == nullptr )
        {
            return;
        }

        mt::vec2f position = m_body->getPosition();
        float angle = m_body->getAngle();

        m_vectorizator->setLocalPosition( { m_worldOffset.x + position.x, m_worldOffset.y + position.y, 0.f } );
        m_vectorizator->setLocalOrientationX( -angle );

        LOGGER_MESSAGE( "%4.2f %4.2f %4.2f", position.x, position.y, angle );
    }
};