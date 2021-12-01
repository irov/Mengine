#include "BubblegumEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"
#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

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
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/TimepipeHelper.h"
#include "Kernel/Vector.h"
#include "Kernel/Stringalized.h"
#include "Kernel/NodeCast.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BubblegumEventReceiver::BubblegumEventReceiver()
        : m_scene( nullptr )
        , m_timepipeId( INVALID_UNIQUE_ID )
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
    }    
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        HotSpotGlobalPtr hotspot = Helper::generateHotSpotGlobal( MENGINE_DOCUMENT_FACTORABLE );

        m_scene->addChild( hotspot );

        m_globalHotspot = hotspot;

        TextFieldPtr textScore = Helper::generateTextField( MENGINE_DOCUMENT_FACTORABLE );

        textScore->setTextId( STRINGIZE_STRING_LOCAL( "ID_Score" ) );

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

            Char arg_buff[16];
            Helper::stringalized( m_score, arg_buff, 16 );

            _arg->assign( arg_buff );

            return true;
        } );

        TransformationInterface * textScoreTransformation = textScore->getTransformation();
        textScoreTransformation->setLocalPosition( {50.f, 50.f, 0.f} );

        m_textScore = textScore;

        m_scene->addChild( m_textScore );

        TextFieldPtr textFinish = Helper::generateTextField( MENGINE_DOCUMENT_FACTORABLE );

        textFinish->setTextId( STRINGIZE_STRING_LOCAL( "ID_Finish" ) );

        textFinish->setTextFormatArgs( empty_args );

        textFinish->setTextFormatArgsContext( 0, [this]( String * _arg )
        {
            static uint32_t cache_score = ~0U;

            if( cache_score == m_score )
            {
                return false;
            }

            cache_score = m_score;

            Char arg_buffer[16];
            Helper::stringalized( m_score, arg_buffer, 16 );

            _arg->assign( arg_buffer );

            return true;
        } );

        TransformationInterface * textFinishTransformation = textFinish->getTransformation();
        textFinishTransformation->setLocalPosition( {300.f, 300.f, 0.f} );

        m_textFinish = textFinish;

        m_scene->addChild( m_textFinish );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::spawnBubble_( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator )
    {
        MENGINE_UNUSED( _source );
        MENGINE_UNUSED( _iterator );

        const RandomizerInterfacePtr & randomizer = PLAYER_SERVICE()
             ->getRandomizer();

        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FACTORABLE );

        float r = randomizer->getRandomRangef( 0.1f, 1.f );
        float g = randomizer->getRandomRangef( 0.2f, 1.f );
        float b = randomizer->getRandomRangef( 0.3f, 1.f );
        
        surface->setSolidColor( {r, g, b, 1.f} );

        float width = 50.f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FACTORABLE );

        shape->setSurface( surface );

        const Resolution & resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float ResolutionWidth = resolution.getWidthF();
        float ResolutionHeight = resolution.getHeightF();
        float x = randomizer->getRandomRangef( width * 2, ResolutionWidth - width * 2 );
        float y = randomizer->getRandomRangef( width * 2, ResolutionHeight - width * 2 );

        TransformationInterface * shapeTransformation = shape->getTransformation();
        shapeTransformation->setLocalPosition( {x, y, 0.f} );
        shapeTransformation->setLocalScale( {0.f, 0.f, 0.f} );

        HotSpotCirclePtr hotspot = Helper::generateHotSpotCircle( MENGINE_DOCUMENT_FACTORABLE );

        hotspot->setRadius( 50.f );

        shape->addChild( hotspot );

        m_scene->addChild( shape );

        auto && [source_scale, source_click, source_burst] = Cook::addRace<3>( _source );
        
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

        Cook::addTransformationScaleTime( source_scale, shape, shape, nullptr, mt::vec3f( 1.f, 1.f, 1.f ), delay, ETASK_FLAG_NOREWIND, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addNoSkip( source_scale );
        Cook::addPrint( source_scale, "source_scale!!!" );
        Cook::addSemaphoreAssign( source_scale, m_semaphoreBurst, 1 );

        Cook::addPickerableMouseButton( source_click, hotspot, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addNodeDisable( source_click, shape );

        Cook::addFunction( source_click, this, &BubblegumEventReceiver::addScore_, shape, _iterator );

        Cook::addSemaphoreEqual( source_burst, m_semaphoreBurst, 1 );
        Cook::addPrint( source_burst, "source_burst!!!" );

        Cook::addNodeDestroy( _source, shape );
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::addScore_( const NodePtr & _shape, uint32_t _iterator )
    {
        uint32_t n = 1 + _iterator / 10;

        int addscore = 100 * n;

        TransformationInterface * shapeTransformation = _shape->getTransformation();

        const mt::vec3f & scale = shapeTransformation->getLocalScale();
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

        GOAP::TimerInterfacePtr timer = GOAP_SERVICE()
            ->makeTimer();

        UniqueId timepipeId = Helper::addTimepipe( [timer]( const UpdateContext * _context )
        {
            float time = _context->time;

            timer->update( time );
        }, MENGINE_DOCUMENT_FACTORABLE );

        m_timepipeId = timepipeId;

        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addNodeDisable( source, m_textFinish );

        Cook::addGlobalDelay( source, 1000.f, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_generator, source_burst, source_missclick] = Cook::addRace<3>( source );
        
        Cook::addGenerator( source_generator, timer, []( uint32_t _iterator )
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
        }, [this]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, float _time )
        {
            MENGINE_UNUSED( _time );

            Cook::addScope( _source, this, &BubblegumEventReceiver::spawnBubble_, _iterator );
        } );

        m_semaphoreBurst = GOAP_SERVICE()
            ->makeSemaphore( 0 );

        Cook::addSemaphoreEqual( source_burst, m_semaphoreBurst, 1 );
        Cook::addPrint( source_burst, "burst" );

        Cook::addPickerableMouseButton( source_missclick, m_globalHotspot, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addPrint( source_missclick, "test" );
        Cook::addSemaphoreAssign( source_missclick, m_semaphoreBurst, 1 );

        Cook::addNodeDisable( source, m_textScore );
        Cook::addNodeEnable( source, m_textFinish );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
        
        if( m_timepipeId != 0 )
        {
            Helper::removeTimepipe( m_timepipeId );
            m_timepipeId = 0;
        }

        m_globalHotspot = nullptr;
        m_semaphoreBurst = nullptr;
        m_textScore = nullptr;
        m_textFinish = nullptr;
    }
};