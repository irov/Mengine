#include "BubblegumEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"

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

#include "Tasks/TaskTransformationScaleTime.h"
#include "Tasks/TaskPickerableMouseButton.h"
#include "Tasks/TaskNodeDisable.h"
#include "Tasks/TaskNodeEnable.h"
#include "Tasks/TaskNodeDestroy.h"
#include "Tasks/TaskGlobalDelay.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BubblegumEventReceiver::BubblegumEventReceiver()
        : m_scene( nullptr )
        , m_score( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BubblegumEventReceiver::~BubblegumEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;

        m_globalHotspot = nullptr;
        m_semaphoreBurst = nullptr;
        m_textScore = nullptr;
        m_textFinish = nullptr;
    }    
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        HotSpotGlobalPtr hotspot = Helper::generateHotSpotGlobal( MENGINE_DOCUMENT_FUNCTION );

        m_scene->addChild( hotspot );

        m_globalHotspot = hotspot;

        TextFieldPtr textScore = Helper::generateTextField( MENGINE_DOCUMENT_FUNCTION );

        textScore->setTextID( STRINGIZE_STRING_LOCAL( "ID_Score" ) );

        VectorString empty_args;
        empty_args.push_back( "" );
        textScore->setTextFormatArgs( empty_args );

        textScore->setTextFormatArgsContext( 0, [this]( String * _arg )
        {
            static uint32_t cache_score = ~0U;

            if( cache_score == m_score )
            {
                return false;
            }

            cache_score = m_score;

            Helper::unsignedToString( m_score, _arg );

            return true;
        } );

        textScore->setLocalPosition( {50.f, 50.f, 0.f} );

        m_textScore = textScore;

        m_scene->addChild( m_textScore );

        TextFieldPtr textFinish = Helper::generateTextField( MENGINE_DOCUMENT_FUNCTION );

        textFinish->setTextID( STRINGIZE_STRING_LOCAL( "ID_Finish" ) );

        textFinish->setTextFormatArgs( empty_args );

        textFinish->setTextFormatArgsContext( 0, [this]( String * _arg )
        {
            static uint32_t cache_score = ~0U;

            if( cache_score == m_score )
            {
                return false;
            }

            cache_score = m_score;

            Helper::unsignedToString( m_score, _arg );

            return true;
        } );

        textFinish->setLocalPosition( {300.f, 300.f, 0.f} );

        m_textFinish = textFinish;

        m_scene->addChild( m_textFinish );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::spawnBubble_( const GOAP::SourcePtr & _source, uint32_t _iterator )
    {
        MENGINE_UNUSED( _source );
        MENGINE_UNUSED( _iterator );

        const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
             ->getRandomizer();

        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

        float r = randomizer->getRandomRangef( 0.3f, 1.f );
        float g = randomizer->getRandomRangef( 0.3f, 1.f );
        float b = randomizer->getRandomRangef( 0.3f, 1.f );
        
        surface->setSolidColor( {r, g, b, 1.f} );

        float width = 50.f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FUNCTION );

        shape->setSurface( surface );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float ResolutionWidth = resolution.getWidthF();
        float ResolutionHeight = resolution.getHeightF();
        float x = randomizer->getRandomRangef( width * 2, ResolutionWidth - width * 2 );
        float y = randomizer->getRandomRangef( width * 2, ResolutionHeight - width * 2 );

        shape->setLocalPosition( {x, y, 0.f} );
        shape->setLocalScale( {0.f, 0.f, 0.f} );

        HotSpotCirclePtr hotspot = Helper::generateHotSpotCircle( MENGINE_DOCUMENT_FUNCTION );

        hotspot->setRadius( 50.f );

        shape->addChild( hotspot );

        m_scene->addChild( shape );

        auto && [source_scale, source_click, source_burst] = _source->addRace<3>();
        
        float delay;

        if( _iterator > 50 )
        {
            delay = 500.f;
        }
        else if( _iterator > 25 )
        {
            delay = 1000.f;
        }
        else if( _iterator > 10 )
        {
            delay = 2000.f;
        }
        else
        {
            delay = 5000.f;
        }

        source_scale->addTask<TaskTransformationScaleTime>( shape, shape, nullptr, mt::vec3f( 1.f, 1.f, 1.f ), delay, ETASK_FLAG_NOREWIND );
        source_scale->addNoSkip();
        source_scale->addSemaphoreAssign( m_semaphoreBurst, 1 );        

        source_click->addTask<TaskPickerableMouseButton>( hotspot, MC_LBUTTON, true, true, nullptr );
        source_click->addTask<TaskNodeDisable>( shape );

        source_click->addFunction( this, &BubblegumEventReceiver::addScore_, shape, _iterator );

        source_burst->addSemaphoreEqual( m_semaphoreBurst, 1 );

        _source->addTask<TaskNodeDestroy>( shape );
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::addScore_( const NodePtr & _shape, uint32_t _iterator )
    {
        uint32_t n = 1 + _iterator / 10;

        int addscore = 100 * n;

        const mt::vec3f & scale = _shape->getLocalScale();
        float coeff = scale.x;

        if( coeff < 0.15f )
        {
            addscore *= 3;
        }
        else if( coeff < 0.35f )
        {
            addscore *= 2;
        }

        m_score += addscore;
    }
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        GOAP::TimerPtr timer = GOAP::Helper::makeTimer();

        Helper::addTimepipe( [timer]( const UpdateContext * _context )
        {
            float time = _context->time;

            timer->update( time );
        } );

        GOAP::SourcePtr source = GOAP::Helper::makeSource();

        source->addTask<TaskNodeDisable>( m_textFinish );

        source->addTask<TaskGlobalDelay>( 1000.f );

        auto && [source_generator, source_burst, source_missclick] = source->addRace<3>();
        
        source_generator->addGenerator( timer, []( uint32_t _iterator )
        {
            MENGINE_UNUSED( _iterator );

            if( _iterator > 50 )
            {
                return 250.f;
            }
            else if( _iterator > 25 )
            {
                return 500.f;   
            }
            else if( _iterator > 10 )
            {
                return 750.f;
            }

            return 1000.f;
        }, [this]( const GOAP::SourcePtr & _source, uint32_t _iterator, float _time )
        {
            MENGINE_UNUSED( _time );

            _source->addScope( this, &BubblegumEventReceiver::spawnBubble_, _iterator );
        } );

        GOAP::EventPtr event = GOAP::Helper::makeEvent();
        m_semaphoreBurst = GOAP::Helper::makeSemaphore( event, 0 );

        source_burst->addSemaphoreEqual( m_semaphoreBurst, 1 );

        source_missclick->addTask<TaskPickerableMouseButton>( m_globalHotspot, MC_LBUTTON, true, true, nullptr );
        source_missclick->addSemaphoreAssign( m_semaphoreBurst, 1 );        

        source->addTask<TaskNodeDisable>( m_textScore );
        source->addTask<TaskNodeEnable>( m_textFinish );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );
        chain->run();

        return true;
    }
};