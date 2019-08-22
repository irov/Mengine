#include "MySceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/Vectorizator.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MySceneEventReceiver::MySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MySceneEventReceiver::~MySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MySceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        LOGGER_MESSAGE( "Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
            ->getScheduler();

        uint32_t id = Helper::schedulerTiming( scheduler
            , []( uint32_t _id, uint32_t _index )
        {
            MENGINE_UNUSED( _id );

            if( _index == 5000 )
            {
                return -1.f;
            }

            return 1.f;
        }, [this]( uint32_t _id, uint32_t _index, float _time )
        {
            MENGINE_UNUSED( _id );
            MENGINE_UNUSED( _index );
            MENGINE_UNUSED( _time );

            SurfaceSolidColorPtr surface = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FUNCTION );

            const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
                ->getRandomizer();

            float r = randomizer->getRandomRangef( 0.f, 1.f );
            float g = randomizer->getRandomRangef( 0.f, 1.f );
            float b = randomizer->getRandomRangef( 0.f, 1.f );
            float a = randomizer->getRandomRangef( 0.f, 1.f );

            surface->setSolidColor( { r, g, b, a } );

            float w = randomizer->getRandomRangef( 20, 200 );
            float h = randomizer->getRandomRangef( 20, 200 );
            surface->setSolidSize( { w, h } );

            ShapePtr shape = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FUNCTION );

            shape->setSurface( surface );

            const Resolution & resolution = APPLICATION_SERVICE()
                ->getContentResolution();

            float ResolutionWidth = resolution.getWidthF();
            float ResolutionHeight = resolution.getHeightF();
            float x = randomizer->getRandomRangef( 0, ResolutionWidth );
            float y = randomizer->getRandomRangef( 0, ResolutionHeight );

            shape->setLocalPosition( { x, y, 0.f } );

            m_scene->addChild( shape );
        }, nullptr );

        MENGINE_UNUSED( id );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        
        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void MySceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
};