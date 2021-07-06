#include "JewelryEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Plugins/JSONPlugin/JSONInterface.h"
#include "Plugins/GOAPPlugin/GOAPInterface.h"
#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Engine/Engine.h"
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
#include "Kernel/FilePathHelper.h"
#include "Kernel/Vector.h"
#include "Kernel/Stringalized.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ShapeCircle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryEventReceiver::JewelryEventReceiver()
        : m_scene( nullptr )
        , m_timepipeId( INVALIDATE_UNIQUE_ID )
        , m_stage( 0 )
        , m_timemillisecond( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryEventReceiver::~JewelryEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        m_factoryJewelry = Helper::makeFactoryPool<Jewelry, 128>( MENGINE_DOCUMENT_FACTORABLE );

        m_randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;

        m_factoryJewelry = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        const SettingInterfacePtr & game_setting = SETTINGS_SERVICE()
            ->getSetting( STRINGIZE_STRING_LOCAL( "Game" ) );

        uint32_t column_count = game_setting->getValue( "jewelry_column_count", 6U );
        uint32_t row_count = game_setting->getValue( "jewelry_row_count", 10U );
        m_settings.m_jewelry_size = game_setting->getValue( "jewelry_size", 50.f );
        m_settings.m_jewelry_stride = game_setting->getValue( "jewelry_stride", 10.f );
        m_settings.m_jewelry_cell_explosive_time_ms = game_setting->getValue( "jewelry_cell_explosive_time_ms", 750.f );
        m_settings.m_jewelry_cell_explosive_count = game_setting->getValue( "jewelry_cell_explosive_count", MENGINE_MAX( column_count, row_count ) );
        m_settings.m_jewelry_collapse = game_setting->getValue( "jewelry_collapse", true );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        m_storageLevels = JSON_SERVICE()
            ->loadJSON( fileGroup, STRINGIZE_FILEPATH_LOCAL( "levels.json" ), MENGINE_DOCUMENT_FACTORABLE );

        m_jewelryMatrix = Helper::makeFactorableUnique<JewelryMatrix>( MENGINE_DOCUMENT_FACTORABLE );

        m_eventFall = GOAP_SERVICE()
            ->makeEvent();

        if( m_jewelryMatrix->initialize( m_eventFall, column_count, row_count ) == false )
        {
            return false;
        }

        m_jewelryHand.clear();

        m_base = Helper::generateNode( MENGINE_DOCUMENT_FACTORABLE );

        TransformationInterface * transformation = m_base->getTransformation();
        transformation->setLocalPosition( {300.f, 100.f, 0.f} );

        m_scene->addChild( m_base );

        m_timemillisecond = TIME_SYSTEM()
            ->getTimeMilliseconds();

        m_stage = 0;

        this->makeUI_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryPtr JewelryEventReceiver::makeJewelry_( EJewelrySuper _super, uint32_t _type, uint32_t _column, uint32_t _row, const DocumentPtr & _doc )
    {
        JewelryPtr jewelry = m_factoryJewelry->createObject( _doc );

        if( jewelry->initialize( _super, _type, m_jewelryMatrix, _column, _row, m_settings.m_jewelry_size, m_settings.m_jewelry_stride ) == false )
        {
            return nullptr;
        }

        m_jewelryMatrix->setJewelry( _column, _row, jewelry );

        const NodePtr & node = jewelry->getNode();

        m_base->addChild( node );

        return jewelry;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::makeUI_()
    {
        this->makeUITextStage_();
        this->makeUITextTime_();
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::makeUITextStage_()
    {
        TextFieldPtr textScore = Helper::generateTextField( MENGINE_DOCUMENT_FACTORABLE );

        textScore->setTextId( STRINGIZE_STRING_LOCAL( "ID_Stage" ) );

        VectorString empty_args;
        empty_args.push_back( "" );
        textScore->setTextFormatArgs( empty_args );

        textScore->setTextFormatArgsContext( 0, [this]( String * _arg )
        {
            static uint32_t cache_stage = ~0U;

            if( cache_stage == m_stage )
            {
                return false;
            }

            cache_stage = m_stage;

            Char arg_buffer[16];
            Helper::stringalized( m_stage, arg_buffer, 16 );

            _arg->assign( arg_buffer );

            return true;
        } );

        TransformationInterface * textScoreTransformation = textScore->getTransformation();
        textScoreTransformation->setLocalPosition( {50.f, 50.f, 0.f} );

        m_textStage = textScore;

        m_scene->addChild( m_textStage );
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::makeUITextTime_()
    {
        TextFieldPtr textTime = Helper::generateTextField( MENGINE_DOCUMENT_FACTORABLE );

        textTime->setTextId( STRINGIZE_STRING_LOCAL( "ID_Time" ) );

        VectorString empty_args;
        empty_args.push_back( "" );
        textTime->setTextFormatArgs( empty_args );

        textTime->setTextFormatArgsContext( 0, [this]( String * _arg )
        {
            static uint64_t cache_time = ~0U;

            uint64_t timemillisecond = TIME_SYSTEM()
                ->getTimeMilliseconds();

            uint64_t timesecond = (timemillisecond - m_timemillisecond) / 1000;

            if( cache_time == timesecond )
            {
                return false;
            }

            cache_time = timesecond;

            Char arg_buffer[16];
            Helper::stringalized( timesecond, arg_buffer, 16 );

            _arg->assign( arg_buffer );

            return true;
        } );

        TransformationInterface * textTimeTransformation = textTime->getTransformation();
        textTimeTransformation->setLocalPosition( {50.f, 150.f, 0.f} );

        m_textTime = textTime;

        m_scene->addChild( m_textTime );
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::spawnJewelry_( const GOAP::SourceInterfacePtr & _source, EJewelrySuper _super, uint32_t _iterator )
    {
        MENGINE_UNUSED( _source );
        MENGINE_UNUSED( _iterator );

        uint32_t jewelry_matrix_column_count = m_jewelryMatrix->getColumnCount();

        uint32_t jewelry_column = m_randomizer->getRandom( jewelry_matrix_column_count );

        if( m_jewelryMatrix->existJewelry( jewelry_column, 0 ) == true )
        {
            return;
        }

        if( m_jewelryMatrix->existJewelry( jewelry_column, 1 ) == true )
        {
            return;
        }

        uint32_t jewelry_type = m_randomizer->getRandom( m_settings.m_jewelry_type_count );

        JewelryPtr jewelry = this->makeJewelry_( _super, jewelry_type, jewelry_column, 0, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_pick, source_fall] = Cook::addRace<2>( _source );

        Cook::addWhile( source_pick, [this, jewelry]( const GOAP::SourceInterfacePtr & _source_pick )
        {
            const PickerablePtr & jewelry_pickerable = jewelry->getPickerable();

            auto && [source_pick_click, source_pick_enter] = Cook::addRace<2>( _source_pick );

            Cook::addPickerableMouseButton( source_pick_click, jewelry_pickerable, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addPickerableMouseEnter( source_pick_enter, jewelry_pickerable, []( const InputMouseEnterEvent & _event, bool * _handle )
            {
                MENGINE_UNUSED( _event );

                if( INPUT_SERVICE()
                    ->isMouseButtonDown( MC_LBUTTON ) == false )
                {
                    return false;
                }

                *_handle = true;

                return true;
            }, MENGINE_DOCUMENT_FACTORABLE );

            Cook::addScope( _source_pick, [this, jewelry]( const GOAP::SourceInterfacePtr & _source )
            {
                if( jewelry->isBlock() == true )
                {
                    return;
                }

                if( jewelry->getSuper() != EJSUPER_NORMAL )
                {
                    return;
                }

                if( m_jewelryHand.empty() == false )
                {
                    if( std::find( m_jewelryHand.begin(), m_jewelryHand.end(), jewelry ) != m_jewelryHand.end() )
                    {
                        return;
                    }

                    const JewelryPtr & hand_jewelry = m_jewelryHand.front();

                    uint32_t hand_type = hand_jewelry->getType();

                    uint32_t jewelry_type = jewelry->getType();

                    if( hand_type != jewelry_type )
                    {
                        return;
                    }
                }

                m_jewelryHand.emplace_back( jewelry );

                jewelry->pickHand( _source );
            } );

            return true;
        } );

        uint32_t jewelry_matrix_row_count = m_jewelryMatrix->getRowCount();

        Cook::addFor( source_fall, jewelry_matrix_row_count - 1, [this, jewelry]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _count );

            uint32_t jewelry_next_row = _iterator + 1;

            if( jewelry->isDead() == true )
            {
                return false;
            }

            Cook::addTrigger( _source, m_eventFall, [this, jewelry, jewelry_next_row]( const GOAP::SourceInterfacePtr & _source )
            {
                if( jewelry->isDead() == true )
                {
                    return false;
                }

                uint32_t jewelry_column = jewelry->getColumn();

                const JewelryPtr & jewelry_next = m_jewelryMatrix->getJewelry( jewelry_column, jewelry_next_row );

                if( jewelry_next != nullptr && jewelry_next->isBlock() == true )
                {
                    EJewelrySuper super = jewelry->getSuper();

                    switch( super )
                    {
                    case EJSUPER_NORMAL:
                        {
                            this->collapseJewelry_( _source, jewelry );
                        }break;
                    case EJSUPER_BOMB:
                        {
                            this->explosiveJewelry_( _source, jewelry );

                            return true;
                        }break;
                    default:
                        {
                            return false;
                        }break;
                    }

                    return false;
                }
                else if( jewelry_next != nullptr )
                {
                    return false;
                }

                return true;
            } );

            Cook::addScope( _source, [this, jewelry, jewelry_next_row]( const GOAP::SourceInterfacePtr & _source )
            {
                jewelry->move( _source, jewelry_next_row, m_settings.m_jewelry_cell_fall_time_ms );
            } );

            return true;
        } );


        Cook::addScope( source_fall, [this, jewelry]( const GOAP::SourceInterfacePtr & _source )
        {
            EJewelrySuper super = jewelry->getSuper();

            switch( super )
            {
            case EJSUPER_NORMAL:
                {
                    this->collapseJewelry_( _source, jewelry );
                }break;
            case EJSUPER_BOMB:
                {
                    this->explosiveJewelry_( _source, jewelry );

                    return true;
                }break;
            default:
                {
                }break;
            }

            return true;
        } );

        Cook::addBlock( source_fall );
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::collapseJewelry_( const GOAP::SourceInterfacePtr & _source, const JewelryPtr & _jewelry )
    {
        if( m_jewelryMatrix->isFall( _jewelry ) == false )
        {
            _jewelry->stop();
            _jewelry->block( _source );
        }

        if( m_settings.m_jewelry_collapse == false )
        {
            return;
        }

        uint32_t jewelry_type = _jewelry->getType();
        uint32_t jewelry_column = _jewelry->getColumn();
        uint32_t jewelry_row = _jewelry->getRow();

        VectorJewelries jewelries;
        m_jewelryMatrix->getNeighbours( jewelry_column, jewelry_row, jewelry_type, jewelries, []( const JewelryPtr & _jewelry )
        {
            if( _jewelry->isBlock() == false )
            {
                return false;
            }

            if( _jewelry->isMove() == true )
            {
                return false;
            }

            return true;
        } );

        if( jewelries.size() < 3 )
        {
            _jewelry->block( _source );

            return;
        }

        const GOAP::SourceInterfacePtr & source_fork = Cook::addFork( _source );

        for( auto && [source_jewelry, jewerly] : Cook::addParallelZip( source_fork, jewelries ) )
        {
            jewerly->dead( source_jewelry );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::explosiveJewelry_( const GOAP::SourceInterfacePtr & _source, const JewelryPtr & _jewelry )
    {
        if( _jewelry->isDead() == true )
        {
            return;
        }

        uint32_t jewelry_matrix_column_count = m_jewelryMatrix->getColumnCount();
        uint32_t jewelry_matrix_row_count = m_jewelryMatrix->getRowCount();

        uint32_t jewelry_column = _jewelry->getColumn();
        uint32_t jewelry_row = _jewelry->getRow();

        _jewelry->explosive( _source );

        auto && [source_left, source_right, source_down] = Cook::addParallel<3>( _source );

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.z = 0.f;

            TransformationInterface * explosiveTransformation = explosive->getTransformation();
            explosiveTransformation->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            Cook::addFor( source_left, 1, MENGINE_MIN( jewelry_column + 1, m_settings.m_jewelry_cell_explosive_count + 1 ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column - _iterator;
                uint32_t explosive_row = jewelry_row;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.z = 0.f;

                Cook::addTransformationTranslateTime( _source, explosive, explosive, nullptr, explosive_position, m_settings.m_jewelry_cell_explosive_time_ms, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( _source, [this, explosive_column, explosive_row]( const GOAP::SourceInterfacePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourceInterfacePtr & source_fork = Cook::addFork( _source );

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            Cook::addNodeDestroy( source_left, explosive );
        }

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.z = 0.f;

            TransformationInterface * explosiveTransformation = explosive->getTransformation();
            explosiveTransformation->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            Cook::addFor( source_right, 1, MENGINE_MIN( m_settings.m_jewelry_cell_explosive_count + 1, jewelry_matrix_column_count - jewelry_column ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column + _iterator;
                uint32_t explosive_row = jewelry_row;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.z = 0.f;

                Cook::addTransformationTranslateTime( _source, explosive, explosive, nullptr, explosive_position, m_settings.m_jewelry_cell_explosive_time_ms, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( _source, [this, explosive_column, explosive_row]( const GOAP::SourceInterfacePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourceInterfacePtr & source_fork = Cook::addFork( _source );

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            Cook::addNodeDestroy( source_right, explosive );
        }

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
            explosive_position.z = 0.f;

            TransformationInterface * explosiveTransformation = explosive->getTransformation();
            explosiveTransformation->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            Cook::addFor( source_down, 1, MENGINE_MIN( m_settings.m_jewelry_cell_explosive_count + 1, jewelry_matrix_row_count - jewelry_row ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column;
                uint32_t explosive_row = jewelry_row + _iterator;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_settings.m_jewelry_size + m_settings.m_jewelry_stride);
                explosive_position.z = 0.f;

                Cook::addTransformationTranslateTime( _source, explosive, explosive, nullptr, explosive_position, m_settings.m_jewelry_cell_explosive_time_ms, MENGINE_DOCUMENT_FACTORABLE );
                Cook::addScope( _source, [this, explosive_column, explosive_row]( const GOAP::SourceInterfacePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourceInterfacePtr & source_fork = Cook::addFork( _source );

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            Cook::addNodeDestroy( source_down, explosive );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
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

        Cook::addGlobalDelay( source, 1000.f, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_stage, source_spawn, source_boom] = Cook::addParallel<3>( source );

        const jpp::object & levels = m_storageLevels->getJSON();

        const jpp::object & level_test = levels["test"];

        for( const jpp::object & stage : jpp::array( level_test["stages"] ) )
        {
            float time = stage["time"];

            Cook::addFunction( source_stage, [this, stage]()
            {
                ++m_stage;

                m_settings.m_jewelry_type_count = stage["jewelry_type_count"];
                m_settings.m_jewelry_cell_fall_time_ms = stage["jewelry_cell_fall_time_ms"];
                m_settings.m_jewelry_spawn_time_ms = stage["jewelry_spawn_time_ms"];
            } );

            Cook::addGlobalDelay( source_stage, time, MENGINE_DOCUMENT_FACTORABLE );
        }

        Cook::addGenerator( source_spawn, timer, [this]( uint32_t _iterator )
        {
            MENGINE_UNUSED( _iterator );

            return m_settings.m_jewelry_spawn_time_ms;
        }, [this]( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator, float _time )
        {
            MENGINE_UNUSED( _time );

            Cook::addScope( _source, this, &JewelryEventReceiver::spawnJewelry_, EJSUPER_NORMAL, _iterator );
        } );

        Cook::addWhile( source_boom, [this]( const GOAP::SourceInterfacePtr & _source_boom )
        {
            Cook::addGlobalMouseButton( _source_boom, MC_LBUTTON, false, nullptr, MENGINE_DOCUMENT_FACTORABLE );
            Cook::addScope( _source_boom, [this]( const GOAP::SourceInterfacePtr & _source )
            {
                VectorJewelryHand jewelryHand = std::move( m_jewelryHand );

                jewelryHand.erase( std::remove_if( jewelryHand.begin(), jewelryHand.end(), []( const JewelryPtr & _jewelry )
                {
                    return _jewelry->isBlock();
                } ), jewelryHand.end() );

                uint32_t jewelry_count = jewelryHand.size();

                if( jewelry_count < 3 )
                {
                    for( auto && [jewelry_source, jewelry] : Cook::addParallelZip( _source, jewelryHand ) )
                    {
                        jewelry->unpickHand( jewelry_source );
                    }

                    m_jewelryHand.clear();

                    return;
                }

                if( jewelry_count == 3 )
                {
                    for( auto && [jewelry_source, jewelry] : Cook::addParallelZip( _source, jewelryHand ) )
                    {
                        jewelry->dead( jewelry_source );
                    }
                }

                if( jewelry_count >= 4 )
                {
                    JewelryPtr jewelry_bomb = jewelryHand.back();
                    jewelryHand.pop_back();

                    auto && [source_dead, source_bomb] = Cook::addParallel<2>( _source );

                    for( auto && [jewelry_source, jewelry] : Cook::addParallelZip( source_dead, jewelryHand ) )
                    {
                        jewelry->dead( jewelry_source );
                    }

                    jewelry_bomb->bomb( source_bomb );
                }
            } );

            return true;
        } );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FILE, MENGINE_CODE_LINE );

        chain->run();

        m_chain = chain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        Helper::removeTimepipe( m_timepipeId );
        m_timepipeId = INVALIDATE_UNIQUE_ID;

        m_textStage->removeFromParent();
        m_textStage = nullptr;

        m_textTime->removeFromParent();
        m_textTime = nullptr;

        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }

        if( m_jewelryMatrix != nullptr )
        {
            m_jewelryMatrix->finalize();
            m_jewelryMatrix = nullptr;
        }

        if( m_base != nullptr )
        {
            m_base->removeFromParent();
            m_base = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr JewelryEventReceiver::spawnExplosive_()
    {
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FACTORABLE );

        surface->setSolidColor( Color( 1.f, 1.f, 1.f, 0.25f ) );

        float width = 15.f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FACTORABLE );

        shape->setSurface( surface );

        return shape;
    }
    //////////////////////////////////////////////////////////////////////////
};