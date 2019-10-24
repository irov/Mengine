#include "Checkbox.h"

#include "Tasks/EngineSource.h"

#include "CheckboxEventReceiverInterface.h"

#include "Kernel/Delegate.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Checkbox::Checkbox()
        : m_value( false )
        , m_state( ECS_APPEAR )
        , m_semaphoreBlock( GOAP::Helper::makeSemaphore( GOAP::Helper::makeEvent(), 0 ) )
        , m_eventValue( GOAP::Helper::makeEvent() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Checkbox::~Checkbox()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::setValue( bool _value )
    {
        if( m_value == _value )
        {
            return;
        }

        m_value = _value;

        m_eventValue->call();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Checkbox::getValue() const
    {
        return m_value;
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::setBlock( bool _block )
    {
        m_semaphoreBlock->setValue( _block == true ? 1 : 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Checkbox::isBlock() const
    {
        int value = m_semaphoreBlock->getValue();

        return value == 1 ? true : false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::setPickerable( const PickerablePtr & _pickerable )
    {
        m_pickerable = _pickerable;
    }
    //////////////////////////////////////////////////////////////////////////
    const PickerablePtr & Checkbox::getPickerable() const
    {
        return m_pickerable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::setStateNode( bool _value, ECheckboxState _state, const NodePtr & _node )
    {
        uint32_t index = (_value == false ? 0 : 1);

        m_nodes[index][_state] = _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Checkbox::getStateNode( bool _value, ECheckboxState _state ) const
    {
        uint32_t index = (_value == false ? 0 : 1);

        return m_nodes[index][_state];
    }
    //////////////////////////////////////////////////////////////////////////
    bool Checkbox::_activate()
    {
        const NodePtr & nodeUnselectIdle = m_nodes[0][ECS_IDLE];

        if( nodeUnselectIdle == nullptr )
        {
            return false;
        }

        const NodePtr & nodeSelectIdle = m_nodes[1][ECS_IDLE];

        if( nodeSelectIdle == nullptr )
        {
            return false;
        }

        if( m_nodes[0][ECS_APPEAR] == nullptr && m_value == false )
        {
            m_state = ECS_IDLE;
        }

        if( m_nodes[1][ECS_APPEAR] == nullptr && m_value == true )
        {
            m_state = ECS_IDLE;
        }

        if( m_nodes[0][ECS_OVER] == nullptr )
        {
            m_nodes[0][ECS_OVER] = m_nodes[0][ECS_IDLE];
        }

        if( m_nodes[1][ECS_OVER] == nullptr )
        {
            m_nodes[1][ECS_OVER] = m_nodes[1][ECS_IDLE];
        }

        EngineSourcePtr source = Helper::makeEngineSource();

        auto fn = [this]( const EngineSourcePtr & _source )
        {
            typedef void (Checkbox:: * TF)(const EngineSourcePtr &, const NodePtr &, bool);

            const TF stateFunctions[] =
            {
                &Checkbox::__stateAppear,
                &Checkbox::__stateIdle,
                &Checkbox::__stateOver,
                &Checkbox::__stateEnter,
                &Checkbox::__stateLeave,
                &Checkbox::__stateClick,
                &Checkbox::__statePush,
                &Checkbox::__stateRelease,
                &Checkbox::__statePressed,
                &Checkbox::__stateBlock,
                &Checkbox::__stateBlockEnter,
                &Checkbox::__stateBlockEnd,
            };

            auto stateFunction = stateFunctions[m_state];

            uint32_t index = (m_value == false ? 0 : 1);

            const NodePtr & node = m_nodes[index][m_state];

            _source->addScope( this, stateFunction, node, m_value );

            return true;
        };

        source->addWhile( fn );

        m_chain = GOAP::Helper::makeChain( source );

        if( m_chain->run() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::_deactivate()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__setState( bool _value, ECheckboxState _state )
    {
        m_value = _value;
        m_state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateAppear( const EngineSourcePtr & _source, const NodePtr & _nodeAppear, bool _value )
    {
        if( _nodeAppear == nullptr )
        {
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeAppear );
        _source->addLocalDelay( 0.f );

        auto && [source_true, source_false] = _source->addIf( [this]()
        {
            return this->isEnable();
        } );

        source_false->addBlock();

        auto && [source_over_click, source_over_enter, source_block, source_play] = _source->addRace<4>();

        source_over_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Checkbox::__setState, _value, ECS_PUSH );

        source_over_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Checkbox::__setState, _value, ECS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        source_play->addAnimatablePlayWait( _nodeAppear, _nodeAppear );
        source_play->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

        _source->addNodeDisable( _nodeAppear );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateIdle( const EngineSourcePtr & _source, const NodePtr & _nodeIdle, bool _value )
    {
        _source->addNodeEnable( _nodeIdle );
        _source->addLocalDelay( 0.f );

        auto && [source_over_click, source_over_enter, source_block] = _source->addRace<3>();

        source_over_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Checkbox::__setState, _value, ECS_PUSH );

        source_over_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Checkbox::__setState, _value, ECS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        _source->addNodeDisable( _nodeIdle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateOver( const EngineSourcePtr & _source, const NodePtr & _nodeOver, bool _value )
    {
        _source->addNodeEnable( _nodeOver );
        _source->addLocalDelay( 0.f );

        auto && [source_over_click, source_over_leave, source_block] = _source->addRace<3>();

        source_over_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Checkbox::__setState, _value, ECS_PUSH );

        source_over_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_over_leave->addFunction( this, &Checkbox::__setState, _value, ECS_LEAVE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        _source->addNodeDisable( _nodeOver );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateEnter( const EngineSourcePtr & _source, const NodePtr & _nodeEnter, bool _value )
    {
        if( _nodeEnter == nullptr )
        {
            _source->addEventable( this, EVENT_CHECKBOX_MOUSE_ENTER, &CheckboxEventReceiverInterface::onCheckboxMouseEnter, _value );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_OVER );

            return;
        }

        _source->addNodeEnable( _nodeEnter );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_CHECKBOX_MOUSE_ENTER, &CheckboxEventReceiverInterface::onCheckboxMouseEnter, _value );

        auto && [source_enter_movie, source_enter_leave, source_enter_click] = _source->addRace<3>();
        source_enter_movie->addAnimatablePlayWait( _nodeEnter, _nodeEnter );
        source_enter_movie->addFunction( this, &Checkbox::__setState, _value, ECS_OVER );

        source_enter_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_enter_leave->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

        source_enter_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_enter_click->addFunction( this, &Checkbox::__setState, _value, ECS_PUSH );

        _source->addNodeDisable( _nodeEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateLeave( const EngineSourcePtr & _source, const NodePtr & _nodeLeave, bool _value )
    {
        if( _nodeLeave == nullptr )
        {
            _source->addEventable( this, EVENT_CHECKBOX_MOUSE_LEAVE, &CheckboxEventReceiverInterface::onCheckboxMouseLeave, _value );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeLeave );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_CHECKBOX_MOUSE_LEAVE, &CheckboxEventReceiverInterface::onCheckboxMouseLeave, _value );

        auto && [source_leave_movie, source_leave_enter] = _source->addRace<2>();
        source_leave_movie->addAnimatablePlayWait( _nodeLeave, _nodeLeave );
        source_leave_movie->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

        source_leave_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_leave_enter->addFunction( this, &Checkbox::__setState, _value, ECS_OVER );

        _source->addNodeDisable( _nodeLeave );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__statePush( const EngineSourcePtr & _source, const NodePtr & _nodePush, bool _value )
    {
        if( _nodePush == nullptr )
        {
            _source->addEventable( this, EVENT_CHECKBOX_MOUSE_PUSH, &CheckboxEventReceiverInterface::onCheckboxMousePush, _value );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_PRESSED );

            return;
        }

        _source->addNodeEnable( _nodePush );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_CHECKBOX_MOUSE_PUSH, &CheckboxEventReceiverInterface::onCheckboxMousePush, _value );

        auto && [source_Push_movie, source_Push_leave, source_Pressed_click_Rel] = _source->addRace<3>();
        source_Push_movie->addAnimatablePlayWait( _nodePush, _nodePush );
        source_Push_movie->addFunction( this, &Checkbox::__setState, _value, ECS_PRESSED );

        source_Push_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_Push_leave->addFunction( this, &Checkbox::__setState, _value, ECS_RELEASE );

        source_Pressed_click_Rel->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Checkbox::__setState, _value, ECS_CLICK );

        _source->addNodeDisable( _nodePush );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__statePressed( const EngineSourcePtr & _source, const NodePtr & _nodePressed, bool _value )
    {
        if( _nodePressed == nullptr )
        {
            _source->addEventable( this, EVENT_CHECKBOX_MOUSE_PRESSED, &CheckboxEventReceiverInterface::onCheckboxMousePressed, _value );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_CLICK );

            return;
        }

        _source->addNodeEnable( _nodePressed );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_CHECKBOX_MOUSE_PRESSED, &CheckboxEventReceiverInterface::onCheckboxMousePressed, _value );

        auto && [source_Pressed_click_Rel, source_Pressed_leave, source_block] = _source->addRace<3>();

        source_Pressed_click_Rel->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Checkbox::__setState, _value, ECS_CLICK );

        source_Pressed_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_Pressed_leave->addFunction( this, &Checkbox::__setState, _value, ECS_RELEASE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        _source->addNodeDisable( _nodePressed );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateRelease( const EngineSourcePtr & _source, const NodePtr & _nodeRelease, bool _value )
    {
        if( _nodeRelease == nullptr )
        {
            _source->addEventable( this, EVENT_CHECKBOX_MOUSE_RELEASE, &CheckboxEventReceiverInterface::onCheckboxMouseRelease, _value );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeRelease );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_CHECKBOX_MOUSE_RELEASE, &CheckboxEventReceiverInterface::onCheckboxMouseRelease, _value );

        auto && [source_Release_movie, source_Release_enter] = _source->addRace<2>();
        source_Release_movie->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, true, nullptr );
        source_Release_movie->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

        source_Release_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_Release_enter->addFunction( this, &Checkbox::__setState, _value, ECS_PRESSED );

        _source->addNodeDisable( _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateClick( const EngineSourcePtr & _source, const NodePtr & _nodeClick, bool _value )
    {
        bool changeValue = !_value;

        if( _nodeClick == nullptr )
        {
            _source->addFunction( this, &Checkbox::__setState, changeValue, ECS_IDLE );
            _source->addEventable( this, EVENT_CHECKBOX_CHANGE, &CheckboxEventReceiverInterface::onCheckboxChange, changeValue );

            return;
        }

        _source->addNodeEnable( _nodeClick );
        _source->addAnimatablePlayWait( _nodeClick, _nodeClick );
        
        _source->addFunction( this, &Checkbox::__setState, changeValue, ECS_IDLE );
        _source->addEventable( this, EVENT_CHECKBOX_CHANGE, &CheckboxEventReceiverInterface::onCheckboxChange, changeValue );

        _source->addNodeDisable( _nodeClick );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlockEnter( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnter, bool _value )
    {
        if( _nodeBlockEnter == nullptr )
        {
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK );

            return;
        }

        _source->addNodeEnable( _nodeBlockEnter );
        _source->addAnimatablePlayWait( _nodeBlockEnter, _nodeBlockEnter );
        _source->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK );

        _source->addNodeDisable( _nodeBlockEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlock( const EngineSourcePtr & _source, const NodePtr & _nodeBlock, bool _value )
    {
        if( _nodeBlock == nullptr )
        {
            _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_END );

            return;
        }

        _source->addNodeEnable( _nodeBlock );

        _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
        _source->addFunction( this, &Checkbox::__setState, _value, ECS_BLOCK_END );

        _source->addNodeDisable( _nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlockEnd( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnd, bool _value )
    {
        if( _nodeBlockEnd == nullptr )
        {
            _source->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeBlockEnd );

        _source->addAnimatablePlayWait( _nodeBlockEnd, _nodeBlockEnd );
        _source->addFunction( this, &Checkbox::__setState, _value, ECS_IDLE );

        _source->addNodeDisable( _nodeBlockEnd );
    }
}