#include "Checkbox.h"

#include "CheckboxEventReceiverInterface.h"

#include "Kernel/Delegate.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Checkbox::Checkbox()
        : m_value( false )
        , m_state( ECS_APPEAR )
        , m_semaphoreBlock( GOAP_SERVICE()->makeSemaphore( 0 ) )
        , m_semaphoreValue( GOAP_SERVICE()->makeSemaphore( 0 ) )
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

        m_semaphoreValue->setValue( 1 );
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

        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        auto fn = [this]( const GOAP::SourceInterfacePtr & _source )
        {
            typedef void (Checkbox:: * TF)(const GOAP::SourceInterfacePtr &, const NodePtr &, bool);

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

            Cook::addSemaphoreAssign( _source, m_semaphoreValue, 0 );
            Cook::addScope( _source, this, stateFunction, node, m_value );

            return true;
        };

        Cook::addWhile( source, fn );

        GOAP::ChainInterfacePtr chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        if( chain->run() == false )
        {
            return false;
        }

        m_chain = chain;

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
    void Checkbox::__stateAppear( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeAppear, bool _value )
    {
        if( _nodeAppear == nullptr )
        {
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeAppear );
        Cook::addLocalDelay( _source, 0.f );

        auto && [source_true, source_false] = Cook::addIf( _source, [this]()
        {
            return this->isEnable();
        } );

        Cook::addBlock( source_false );

        auto && [source_over_click, source_over_enter, source_block, source_play, source_value] = Cook::addRace<5>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr );
        Cook::addFunction( source_over_click, this, &Checkbox::__setState, _value, ECS_PUSH );

        Cook::addPickerableMouseEnter( source_over_enter, m_pickerable, nullptr );
        Cook::addFunction( source_over_enter, this, &Checkbox::__setState, _value, ECS_ENTER );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        Cook::addAnimatablePlayWait( source_play, _nodeAppear, _nodeAppear );
        Cook::addFunction( source_play, this, &Checkbox::__setState, _value, ECS_IDLE );

        Cook::addNodeDisable( _source, _nodeAppear );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateIdle( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeIdle, bool _value )
    {
        Cook::addNodeEnable( _source, _nodeIdle );
        Cook::addLocalDelay( _source, 0.f );

        auto && [source_over_click, source_over_enter, source_block, source_value] = Cook::addRace<4>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr );
        Cook::addFunction( source_over_click, this, &Checkbox::__setState, _value, ECS_PUSH );

        Cook::addPickerableMouseEnter( source_over_enter, m_pickerable, nullptr );
        Cook::addFunction( source_over_enter, this, &Checkbox::__setState, _value, ECS_ENTER );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodeIdle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateOver( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeOver, bool _value )
    {
        Cook::addNodeEnable( _source, _nodeOver );
        Cook::addLocalDelay( _source, 0.f );

        auto && [source_over_click, source_over_leave, source_block, source_value] = Cook::addRace<4>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr );
        Cook::addFunction( source_over_click, this, &Checkbox::__setState, _value, ECS_PUSH );

        Cook::addPickerableMouseLeave( source_over_leave, m_pickerable, nullptr );
        Cook::addFunction( source_over_leave, this, &Checkbox::__setState, _value, ECS_LEAVE );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodeOver );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeEnter, bool _value )
    {
        if( _nodeEnter == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_ENTER, &CheckboxEventReceiverInterface::onCheckboxMouseEnter, _value );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_OVER );

            return;
        }

        Cook::addNodeEnable( _source, _nodeEnter );
        Cook::addLocalDelay( _source, 0.f );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_ENTER, &CheckboxEventReceiverInterface::onCheckboxMouseEnter, _value );

        auto && [source_enter_movie, source_enter_leave, source_enter_click, source_value] = Cook::addRace<4>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addAnimatablePlayWait( source_enter_movie, _nodeEnter, _nodeEnter );
        Cook::addFunction( source_enter_movie, this, &Checkbox::__setState, _value, ECS_OVER );

        Cook::addPickerableMouseLeave( source_enter_leave, m_pickerable, nullptr );
        Cook::addFunction( source_enter_leave, this, &Checkbox::__setState, _value, ECS_IDLE );

        Cook::addPickerableMouseButton( source_enter_click, m_pickerable, MC_LBUTTON, true, true, nullptr );
        Cook::addFunction( source_enter_click, this, &Checkbox::__setState, _value, ECS_PUSH );

        Cook::addNodeDisable( _source, _nodeEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateLeave( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeLeave, bool _value )
    {
        if( _nodeLeave == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_LEAVE, &CheckboxEventReceiverInterface::onCheckboxMouseLeave, _value );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeLeave );
        Cook::addLocalDelay( _source, 0.f );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_LEAVE, &CheckboxEventReceiverInterface::onCheckboxMouseLeave, _value );

        auto && [source_leave_movie, source_leave_enter, source_value] = Cook::addRace<3>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addAnimatablePlayWait( source_leave_movie, _nodeLeave, _nodeLeave );
        Cook::addFunction( source_leave_movie, this, &Checkbox::__setState, _value, ECS_IDLE );

        Cook::addPickerableMouseEnter( source_leave_enter, m_pickerable, nullptr );
        Cook::addFunction( source_leave_enter, this, &Checkbox::__setState, _value, ECS_OVER );

        Cook::addNodeDisable( _source, _nodeLeave );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__statePush( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePush, bool _value )
    {
        if( _nodePush == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_PUSH, &CheckboxEventReceiverInterface::onCheckboxMousePush, _value );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_PRESSED );

            return;
        }

        Cook::addNodeEnable( _source, _nodePush );
        Cook::addLocalDelay( _source, 0.f );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_PUSH, &CheckboxEventReceiverInterface::onCheckboxMousePush, _value );

        auto && [source_Push_movie, source_Push_leave, source_Pressed_click_Rel, source_value] = Cook::addRace<4>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addAnimatablePlayWait( source_Push_movie, _nodePush, _nodePush );
        Cook::addFunction( source_Push_movie, this, &Checkbox::__setState, _value, ECS_PRESSED );

        Cook::addPickerableMouseLeave( source_Push_leave, m_pickerable, nullptr );
        Cook::addFunction( source_Push_leave, this, &Checkbox::__setState, _value, ECS_RELEASE );

        Cook::addPickerableMouseButton( source_Pressed_click_Rel, m_pickerable, MC_LBUTTON, false, false, nullptr );
        Cook::addFunction( source_Pressed_click_Rel, this, &Checkbox::__setState, _value, ECS_CLICK );

        Cook::addNodeDisable( _source, _nodePush );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__statePressed( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePressed, bool _value )
    {
        if( _nodePressed == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_PRESSED, &CheckboxEventReceiverInterface::onCheckboxMousePressed, _value );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_CLICK );

            return;
        }

        Cook::addNodeEnable( _source, _nodePressed );
        Cook::addLocalDelay( _source, 0.f );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_PRESSED, &CheckboxEventReceiverInterface::onCheckboxMousePressed, _value );

        auto && [source_Pressed_click_Rel, source_Pressed_leave, source_block, source_value] = Cook::addRace<4>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addPickerableMouseButton( source_Pressed_click_Rel, m_pickerable, MC_LBUTTON, false, false, nullptr );
        Cook::addFunction( source_Pressed_click_Rel, this, &Checkbox::__setState, _value, ECS_CLICK );

        Cook::addPickerableMouseLeave( source_Pressed_leave, m_pickerable, nullptr );
        Cook::addFunction( source_Pressed_leave, this, &Checkbox::__setState, _value, ECS_RELEASE );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Checkbox::__setState, _value, ECS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodePressed );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateRelease( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease, bool _value )
    {
        if( _nodeRelease == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_RELEASE, &CheckboxEventReceiverInterface::onCheckboxMouseRelease, _value );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeRelease );
        Cook::addLocalDelay( _source, 0.f );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_MOUSE_RELEASE, &CheckboxEventReceiverInterface::onCheckboxMouseRelease, _value );

        auto && [source_Release_movie, source_Release_enter, source_value] = Cook::addRace<3>( _source );

        Cook::addSemaphoreEqual( source_value, m_semaphoreValue, 1 );

        Cook::addPickerableMouseButton( source_Release_movie, m_pickerable, MC_LBUTTON, false, true, nullptr );
        Cook::addFunction( source_Release_movie, this, &Checkbox::__setState, _value, ECS_IDLE );

        Cook::addPickerableMouseEnter( source_Release_enter, m_pickerable, nullptr );
        Cook::addFunction( source_Release_enter, this, &Checkbox::__setState, _value, ECS_PRESSED );

        Cook::addNodeDisable( _source, _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateClick( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeClick, bool _value )
    {
        bool changeValue = !_value;

        if( _nodeClick == nullptr )
        {
            Cook::addFunction( _source, this, &Checkbox::__setState, changeValue, ECS_IDLE );
            Cook::addEventable( _source, this, EVENT_CHECKBOX_CHANGE, &CheckboxEventReceiverInterface::onCheckboxChange, changeValue );

            return;
        }

        Cook::addNodeEnable( _source, _nodeClick );
        Cook::addAnimatablePlayWait( _source, _nodeClick, _nodeClick );
        
        Cook::addFunction( _source, this, &Checkbox::__setState, changeValue, ECS_IDLE );
        Cook::addEventable( _source, this, EVENT_CHECKBOX_CHANGE, &CheckboxEventReceiverInterface::onCheckboxChange, changeValue );

        Cook::addNodeDisable( _source, _nodeClick );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlockEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnter, bool _value )
    {
        if( _nodeBlockEnter == nullptr )
        {
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_BLOCK );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlockEnter );
        Cook::addAnimatablePlayWait( _source, _nodeBlockEnter, _nodeBlockEnter );
        Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_BLOCK );

        Cook::addNodeDisable( _source, _nodeBlockEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlock( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlock, bool _value )
    {
        if( _nodeBlock == nullptr )
        {
            Cook::addSemaphoreEqual( _source, m_semaphoreBlock, 0 );
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_BLOCK_END );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlock );

        Cook::addSemaphoreEqual( _source, m_semaphoreBlock, 0 );
        Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_BLOCK_END );

        Cook::addNodeDisable( _source, _nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Checkbox::__stateBlockEnd( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnd, bool _value )
    {
        if( _nodeBlockEnd == nullptr )
        {
            Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlockEnd );

        Cook::addAnimatablePlayWait( _source, _nodeBlockEnd, _nodeBlockEnd );
        Cook::addFunction( _source, this, &Checkbox::__setState, _value, ECS_IDLE );

        Cook::addNodeDisable( _source, _nodeBlockEnd );
    }
}