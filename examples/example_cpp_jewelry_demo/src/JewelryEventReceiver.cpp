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
        , m_column( 0 )
        , m_row( 0 )
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

        m_column = game_setting->getValue( "jewelry_column", 6U );
        m_row = game_setting->getValue( "jewelry_row", 10U );
        m_count = game_setting->getValue( "jewelry_count", 6U );
        m_cell_fall_time_ms = game_setting->getValue( "jewelry_cell_fall_time_ms", 750.f );
        m_cell_explosive_time_ms = game_setting->getValue( "jewelry_cell_explosive_time_ms", 750.f );

        m_offset = mt::vec3f( 300.f, 100.f, 0.f );

        m_jewelrySlots.resize( m_column * m_row );

        for( JewelrySlot & slot : m_jewelrySlots )
        {
            slot.jewelry = nullptr;
        }

        m_eventFall = GOAP::Helper::makeEvent();

        m_jewelryHand.clear();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JewelryEventReceiver::spawnJewelry_( const GOAP::SourcePtr & _source, uint32_t _iterator )
    {
        MENGINE_UNUSED( _source );
        MENGINE_UNUSED( _iterator );

        uint32_t jewelry_line = m_randomizer->getRandom( m_column );

        JewelrySlot & now_slot = m_jewelrySlots[jewelry_line];

        if( now_slot.jewelry != nullptr )
        {
            return;
        }

        JewelryPtr jewelry = m_factoryJewelry->createObject( MENGINE_DOCUMENT_FUNCTION );

        uint32_t jewelry_type = m_randomizer->getRandom( m_count );

        jewelry->initialize( jewelry_line, jewelry_type );

        const NodePtr & jewelry_node = jewelry->getNode();

        jewelry_node->setLocalPosition( {m_offset.x + float( jewelry_line ) * 60.f, m_offset.y, 0.f} );

        m_scene->addChild( jewelry_node );

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

                const NodePtr & nodeActive = jewelry->getNodeActive();

                _source->addTask<TaskTransformationScaleTime>( nodeActive, nodeActive, nullptr, mt::vec3f( 1.2f, 1.2f, 1.2f ), 200.f );
            } );

            return true;
        } );

        source_fall->addFor( m_row, [this, jewelry, jewelry_node, jewelry_line]( const GOAP::SourcePtr & _source_fall, uint32_t _iterator, uint32_t _count )
        {
            MENGINE_UNUSED( _count );

            if( jewelry->isDead() == true )
            {
                return false;
            }

            _source_fall->addTrigger( m_eventFall, [this, jewelry, jewelry_node, jewelry_line, _iterator]( const GOAP::SourcePtr & _source )
            {
                if( jewelry->isDead() == true )
                {
                    return false;
                }

                uint32_t next_index = jewelry_line + m_column * _iterator;
                JewelrySlot & next_slot = m_jewelrySlots[next_index];

                if( next_slot.jewelry != nullptr && next_slot.jewelry->isBlock() == true )
                {
                    if( jewelry->isBlock() == false && jewelry->isBomb() == false )
                    {
                        const NodePtr & node = jewelry->getNodeActive();

                        _source->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 1.0f, 1.0f, 1.0f ), 100.f );

                        jewelry->block();
                    }

                    if( jewelry->isBomb() == true )
                    {
                        const NodePtr & node_bomb = jewelry->getNodeBomb();

                        _source->addTask<TaskTransformationScaleTime>( node_bomb, node_bomb, nullptr, mt::vec3f( 1.2f, 1.2f, 1.2f ), 200.f );

                        auto && [source_dead, source_left, source_right] = _source->addParallel<3>();

                        uint32_t jewelry_iterator = _iterator;

                        {
                            jewelry->setDead( true );

                            uint32_t jewelry_index = jewelry_line + m_column * jewelry_iterator;
                            JewelrySlot & jewelry_slot = m_jewelrySlots[jewelry_index];

                            jewelry_slot.jewelry = nullptr;

                            source_dead->addTask<TaskTransformationScaleTime>( node_bomb, node_bomb, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f );

                            source_dead->addFunction( [jewelry]()
                            {
                                jewelry->finalize();
                            } );
                        }

                        {
                            NodePtr explosive = this->spawnExplosive_();

                            mt::vec3f explosive_position;
                            explosive_position.x = m_offset.x + float( jewelry_line ) * 60.f;
                            explosive_position.y = m_offset.y + float( jewelry_iterator ) * 60.f;
                            explosive_position.z = 0.f;

                            explosive->setLocalPosition( explosive_position );

                            m_scene->addChild( explosive );

                            source_left->addFor( 1, jewelry_line + 1, [this, explosive, jewelry_line, jewelry_iterator]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
                            {
                                MENGINE_UNUSED( _count );

                                uint32_t explosive_line = jewelry_line - _iterator;
                                uint32_t explosive_iterator = jewelry_iterator;

                                mt::vec3f explosive_position;
                                explosive_position.x = m_offset.x + float( explosive_line ) * 60.f;
                                explosive_position.y = m_offset.y + float( explosive_iterator ) * 60.f;
                                explosive_position.z = 0.f;

                                _source->addTask<TaskTransformationTranslateTime>( explosive, explosive, nullptr, explosive_position, m_cell_explosive_time_ms );
                                _source->addScope( [this, explosive_line, explosive_iterator]( const GOAP::SourcePtr & _source )
                                {
                                    uint32_t jewelry_index = explosive_line + m_column * (explosive_iterator - 1);
                                    JewelrySlot & jewelry_slot = m_jewelrySlots[jewelry_index];

                                    JewelryPtr jewelry = jewelry_slot.jewelry;

                                    if( jewelry == nullptr || jewelry->isBlock() == false )
                                    {
                                        return;
                                    }

                                    jewelry->setDead( true );
                                    jewelry_slot.jewelry = nullptr;

                                    const GOAP::SourcePtr & source_fork = _source->addFork();

                                    const NodePtr & node = jewelry->getNodeActive();

                                    source_fork->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f );

                                    source_fork->addFunction( [jewelry]()
                                    {
                                        jewelry->finalize();
                                    } );
                                } );

                                return true;
                            } );

                            source_left->addTask<TaskNodeDestroy>( explosive );
                        }

                        {
                            NodePtr explosive = this->spawnExplosive_();

                            mt::vec3f explosive_position;
                            explosive_position.x = m_offset.x + float( jewelry_line ) * 60.f;
                            explosive_position.y = m_offset.y + float( jewelry_iterator ) * 60.f;
                            explosive_position.z = 0.f;

                            explosive->setLocalPosition( explosive_position );

                            m_scene->addChild( explosive );

                            source_right->addFor( 1, m_column - jewelry_line, [this, explosive, jewelry_line, jewelry_iterator]( const GOAP::SourcePtr & _source, uint32_t _iterator, uint32_t _count )
                            {
                                MENGINE_UNUSED( _count );

                                uint32_t explosive_line = jewelry_line + _iterator;
                                uint32_t explosive_iterator = jewelry_iterator;

                                mt::vec3f explosive_position;
                                explosive_position.x = m_offset.x + float( explosive_line ) * 60.f;
                                explosive_position.y = m_offset.y + float( explosive_iterator ) * 60.f;
                                explosive_position.z = 0.f;

                                _source->addTask<TaskTransformationTranslateTime>( explosive, explosive, nullptr, explosive_position, m_cell_explosive_time_ms );
                                _source->addScope( [this, explosive_line, explosive_iterator]( const GOAP::SourcePtr & _source )
                                {
                                    uint32_t jewelry_index = explosive_line + m_column * (explosive_iterator - 1);
                                    JewelrySlot & jewelry_slot = m_jewelrySlots[jewelry_index];

                                    JewelryPtr jewelry = jewelry_slot.jewelry;

                                    if( jewelry == nullptr || jewelry->isBlock() == false )
                                    {
                                        return;
                                    }

                                    jewelry->setDead( true );
                                    jewelry_slot.jewelry = nullptr;

                                    const GOAP::SourcePtr & source_fork = _source->addFork();

                                    const NodePtr & node = jewelry->getNodeActive();

                                    source_fork->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f );

                                    source_fork->addFunction( [jewelry]()
                                    {
                                        jewelry->finalize();
                                    } );
                                } );

                                return true;
                            } );

                            source_right->addTask<TaskNodeDestroy>( explosive );
                        }
                    }

                    return false;
                }

                next_slot.jewelry = jewelry;

                jewelry->setIterator( _iterator );

                if( _iterator > 0 )
                {
                    uint32_t prev_index = jewelry_line + m_column * (_iterator - 1);
                    JewelrySlot & prev_slot = m_jewelrySlots[prev_index];

                    prev_slot.jewelry = nullptr;

                    m_eventFall->call();
                }

                return true;
            } );

            mt::vec3f new_position;
            new_position.x = m_offset.x + float( jewelry_line ) * 60.f;
            new_position.y = m_offset.y + float( _iterator + 1 ) * 60.f;
            new_position.z = 0.f;

            _source_fall->addTask<TaskTransformationTranslateTime>( jewelry_node, jewelry_node, nullptr, new_position, m_cell_fall_time_ms );

            return true;
        } );

        const NodePtr & node = jewelry->getNodeActive();

        source_fall->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 1.0f, 1.0f, 1.0f ), 100.f );

        source_fall->addFunction( [jewelry]()
        {
            jewelry->block();
        } );

        source_fall->addBlock();
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

            return 350.f;
        }, [this]( const GOAP::SourcePtr & _source, uint32_t _iterator, float _time )
        {
            MENGINE_UNUSED( _time );

            _source->addScope( this, &JewelryEventReceiver::spawnJewelry_, _iterator );
        } );

        source_boom->addWhile( [this]( const GOAP::SourcePtr & _source_boom )
        {
            _source_boom->addTask<TaskGlobalMouseButton>( MC_LBUTTON, false, nullptr );
            _source_boom->addScope( [this]( const GOAP::SourcePtr & _source )
            {
                m_jewelryHand.erase( std::remove_if( m_jewelryHand.begin(), m_jewelryHand.end(), []( const JewelryPtr & _jewelry )
                {
                    return _jewelry->isBlock();
                } ), m_jewelryHand.end() );

                uint32_t jewelry_count = m_jewelryHand.size();

                if( jewelry_count < 3 )
                {
                    for( const JewelryPtr & jewelry : m_jewelryHand )
                    {
                        const NodePtr & node = jewelry->getNodeActive();

                        _source->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 1.0f, 1.0f, 1.0f ), 200.f );
                    }

                    m_jewelryHand.clear();

                    return;
                }

                VectorJewelryHand jewelryHand = std::move( m_jewelryHand );

                if( jewelry_count >= 4 )
                {
                    JewelryPtr jewelry = jewelryHand.front();
                    jewelryHand.erase( jewelryHand.begin() );
                    jewelry->bomb();
                }

                for( auto && [jewelry_source, jewelry] : _source->addParallelZip( jewelryHand ) )
                {
                    jewelry_source->addFunction( [jewelry]()
                    {
                        jewelry->setDead( true );
                    } );

                    const NodePtr & node = jewelry->getNodeActive();

                    jewelry_source->addTask<TaskTransformationScaleTime>( node, node, nullptr, mt::vec3f( 0.0f, 0.0f, 0.0f ), 200.f );

                    jewelry_source->addFunction( [jewelry]()
                    {
                        jewelry->finalize();
                    } );
                }

                _source->addFunction( [this, jewelryHand]()
                {
                    for( const JewelryPtr & jewelry : jewelryHand )
                    {
                        uint32_t jewelry_line = jewelry->getLine();
                        uint32_t jewelry_iterator = jewelry->getIterator();

                        uint32_t jewelry_index = jewelry_line + m_column * jewelry_iterator;
                        JewelrySlot & jewelry_slot = m_jewelrySlots[jewelry_index];

                        jewelry_slot.jewelry = nullptr;
                    };
                } );

                m_eventFall->call();
            } );

            return true;
        } );

        GOAP::ChainPtr chain = GOAP::Helper::makeChain( source );
        chain->run();

        return true;
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