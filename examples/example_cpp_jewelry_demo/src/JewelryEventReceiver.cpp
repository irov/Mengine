#include "JewelryEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/SettingsServiceInterface.h"

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

#include "Tasks/TaskTransformationTranslateTime.h"
#include "Tasks/TaskTransformationScaleTime.h"
#include "Tasks/TaskPickerableMouseEnter.h"
#include "Tasks/TaskPickerableMouseButton.h"
#include "Tasks/TaskNodeDisable.h"
#include "Tasks/TaskNodeEnable.h"
#include "Tasks/TaskNodeDestroy.h"
#include "Tasks/TaskGlobalMouseButton.h"
#include "Tasks/TaskGlobalDelay.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JewelryEventReceiver::JewelryEventReceiver()
        : m_scene( nullptr )
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

        m_factoryJewelry = Helper::makeFactoryPool<Jewelry, 128>();

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
        m_jewelry_type_count = game_setting->getValue( "jewelry_type_count", 6U );
        m_jewelry_size = game_setting->getValue( "jewelry_size", 50.f );
        m_jewelry_stride = game_setting->getValue( "jewelry_stride", 10.f );
        m_jewelry_cell_fall_time_ms = game_setting->getValue( "jewelry_cell_fall_time_ms", 750.f );
        m_jewelry_cell_explosive_time_ms = game_setting->getValue( "jewelry_cell_explosive_time_ms", 750.f );
        m_jewelry_cell_explosive_count = game_setting->getValue( "jewelry_cell_explosive_count", MENGINE_MAX( column_count, row_count ) );
        m_jewelry_spawn_time_ms = game_setting->getValue( "jewelry_spawn_time_ms", 350.f );

        m_jewelryMatrix = Helper::makeFactorableUnique<JewelryMatrix>();

        m_eventFall = GOAP::Helper::makeEvent();

        if( m_jewelryMatrix->initialize( m_eventFall, column_count, row_count ) == false )
        {
            return false;
        }

        m_jewelryHand.clear();

        m_base = Helper::generateNode( MENGINE_DOCUMENT_FUNCTION );

        m_base->setLocalPosition( { 300.f, 100.f, 0.f } );

        m_scene->addChild( m_base );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    JewelryPtr JewelryEventReceiver::makeJewelry_( EJewelrySuper _super, uint32_t _type, uint32_t _column, uint32_t _row, const Char * _doc )
    {
        JewelryPtr jewelry = m_factoryJewelry->createObject( _doc );

        if( jewelry->initialize( _super, _type, m_jewelryMatrix, _column, _row, m_jewelry_size, m_jewelry_stride ) == false )
        {
            return nullptr;
        }

        m_jewelryMatrix->setJewelry( _column, _row, jewelry );

        const NodePtr & node = jewelry->getNode();

        m_base->addChild( node );

        return jewelry;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::spawnJewelry_( const GOAP::SourcePtr & _source, EJewelrySuper _super, uint32_t _iterator )
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

        uint32_t jewelry_type = m_randomizer->getRandom( m_jewelry_type_count );

        JewelryPtr jewelry = this->makeJewelry_( _super, jewelry_type, jewelry_column, 0, MENGINE_DOCUMENT_FUNCTION );

        auto && [source_pick, source_fall] = _source->addRace<2>();

        source_pick->addWhile( [this, jewelry]( const GOAP::SourcePtr & _source_pick )
        {
            const PickerablePtr & jewelry_pickerable = jewelry->getPickerable();

            auto && [source_pick_click, source_pick_enter] = _source_pick->addRace<2>();

            source_pick_click->addTask<TaskPickerableMouseButton>( jewelry_pickerable, MC_LBUTTON, true, true, nullptr );
            source_pick_enter->addTask<TaskPickerableMouseEnter>( jewelry_pickerable, []( const InputMouseEnterEvent & _event, bool * _handle )
            {
                MENGINE_UNUSED( _event );

                if( INPUT_SERVICE()
                    ->isMouseButtonDown( MC_LBUTTON ) == false )
                {
                    return false;
                }

                *_handle = true;

                return true;
            } );

            _source_pick->addScope( [this, jewelry]( const GOAP::SourcePtr & _source )
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

        source_fall->addFor( jewelry_matrix_row_count - 1, [this, jewelry]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _count );

            uint32_t jewelry_next_row = _iterator + 1;

            if( jewelry->isDead() == true )
            {
                return false;
            }

            _source->addTrigger( m_eventFall, [this, jewelry, jewelry_next_row]( const GOAP::SourcePtr & _source )
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
                            jewelry->block( _source );
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

            _source->addScope( [this, jewelry, jewelry_next_row]( const GOAP::SourcePtr & _source )
            {
                jewelry->move( _source, jewelry_next_row, m_jewelry_cell_fall_time_ms );
            } );

            return true;
        } );


        source_fall->addScope( [this, jewelry]( const GOAP::SourcePtr & _source )
        {
            EJewelrySuper super = jewelry->getSuper();

            switch( super )
            {
            case EJSUPER_NORMAL:
                {
                    jewelry->block( _source );
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

        source_fall->addBlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::explosiveJewelry_( const GOAP::SourcePtr & _source, const JewelryPtr & _jewelry )
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

        auto && [source_left, source_right, source_down] = _source->addParallel<3>();

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.z = 0.f;

            explosive->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            source_left->addFor( 1, MENGINE_MIN( jewelry_column + 1, m_jewelry_cell_explosive_count + 1 ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column - _iterator;
                uint32_t explosive_row = jewelry_row;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.z = 0.f;

                _source->addTask<TaskTransformationTranslateTime>( explosive, explosive, nullptr, explosive_position, m_jewelry_cell_explosive_time_ms );
                _source->addScope( [this, explosive_column, explosive_row]( const GOAP::SourcePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourcePtr & source_fork = _source->addFork();

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            source_left->addTask<TaskNodeDestroy>( explosive );
        }

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.z = 0.f;

            explosive->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            source_right->addFor( 1, MENGINE_MIN( m_jewelry_cell_explosive_count + 1, jewelry_matrix_column_count - jewelry_column ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column + _iterator;
                uint32_t explosive_row = jewelry_row;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.z = 0.f;

                _source->addTask<TaskTransformationTranslateTime>( explosive, explosive, nullptr, explosive_position, m_jewelry_cell_explosive_time_ms );
                _source->addScope( [this, explosive_column, explosive_row]( const GOAP::SourcePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourcePtr & source_fork = _source->addFork();

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            source_right->addTask<TaskNodeDestroy>( explosive );
        }

        {
            NodePtr explosive = this->spawnExplosive_();

            mt::vec3f explosive_position;
            explosive_position.x = float( jewelry_column ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.y = float( jewelry_row ) * (m_jewelry_size + m_jewelry_stride);
            explosive_position.z = 0.f;

            explosive->setLocalPosition( explosive_position );

            m_base->addChild( explosive );

            source_down->addFor( 1, MENGINE_MIN( m_jewelry_cell_explosive_count + 1, jewelry_matrix_row_count - jewelry_row ), [this, explosive, jewelry_column, jewelry_row]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
            {
                MENGINE_UNUSED( _count );

                uint32_t explosive_column = jewelry_column;
                uint32_t explosive_row = jewelry_row + _iterator;

                mt::vec3f explosive_position;
                explosive_position.x = float( explosive_column ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.y = float( explosive_row ) * (m_jewelry_size + m_jewelry_stride);
                explosive_position.z = 0.f;

                _source->addTask<TaskTransformationTranslateTime>( explosive, explosive, nullptr, explosive_position, m_jewelry_cell_explosive_time_ms );
                _source->addScope( [this, explosive_column, explosive_row]( const GOAP::SourcePtr & _source )
                {
                    const JewelryPtr & jewelry = m_jewelryMatrix->getJewelry( explosive_column, explosive_row );

                    if( jewelry == nullptr || jewelry->isBlock() == false )
                    {
                        return;
                    }

                    const GOAP::SourcePtr & source_fork = _source->addFork();

                    jewelry->dead( source_fork );
                } );

                return true;
            } );

            source_down->addTask<TaskNodeDestroy>( explosive );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool JewelryEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
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

        source->addTask<TaskGlobalDelay>( 1000.f );

        auto && [source_spawn, source_boom] = source->addParallel<2>();

        source_spawn->addGenerator( timer, [this]( uint32_t _iterator )
        {
            MENGINE_UNUSED( _iterator );

            return m_jewelry_spawn_time_ms;
        }, [this]( const GOAP::SourcePtr & _source, uint32_t _iterator, float _time )
        {
            MENGINE_UNUSED( _time );

            _source->addScope( this, &JewelryEventReceiver::spawnJewelry_, EJSUPER_NORMAL, _iterator );
        } );

        source_boom->addWhile( [this]( const GOAP::SourcePtr & _source_boom )
        {
            _source_boom->addTask<TaskGlobalMouseButton>( MC_LBUTTON, false, nullptr );
            _source_boom->addScope( [this]( const GOAP::SourcePtr & _source )
            {
                VectorJewelryHand jewelryHand = std::move( m_jewelryHand );

                jewelryHand.erase( std::remove_if( jewelryHand.begin(), jewelryHand.end(), []( const JewelryPtr & _jewelry )
                {
                    return _jewelry->isBlock();
                } ), jewelryHand.end() );

                uint32_t jewelry_count = jewelryHand.size();

                if( jewelry_count < 3 )
                {
                    for( auto && [jewelry_source, jewelry] : _source->addParallelZip( jewelryHand ) )
                    {
                        jewelry->unpickHand( jewelry_source );
                    }

                    m_jewelryHand.clear();

                    return;
                }

                if( jewelry_count == 3 )
                {
                    for( auto && [jewelry_source, jewelry] : _source->addParallelZip( jewelryHand ) )
                    {
                        jewelry->dead( jewelry_source );
                    }
                }

                if( jewelry_count >= 4 )
                {
                    JewelryPtr jewelry_bomb = jewelryHand.back();
                    jewelryHand.pop_back();

                    auto && [source_dead, source_bomb] = _source->addParallel<2>();

                    for( auto && [jewelry_source, jewelry] : source_dead->addParallelZip( jewelryHand ) )
                    {
                        jewelry->dead( jewelry_source );
                    }

                    jewelry_bomb->bomb( source_bomb );
                }
            } );

            return true;
        } );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );
        chain->run();

        m_chain = chain;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

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
        SurfaceSolidColorPtr surface = Helper::generateSurfaceSolidColor( MENGINE_DOCUMENT_FUNCTION );

        surface->setSolidColor( Color( 1.f, 1.f, 1.f, 0.25f ) );

        float width = 15.f;
        surface->setSolidSize( {width, width} );

        ShapeCirclePtr shape = Helper::generateShapeCircle( MENGINE_DOCUMENT_FUNCTION );

        shape->setSurface( surface );

        return shape;
    }
};