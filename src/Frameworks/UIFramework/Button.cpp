#include "Button.h"

#include "Tasks/EngineSource.h"

#include "ButtonEventReceiverInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Button::Button()
        : m_state( EBS_APPEAR )
        , m_semaphoreBlock( GOAP::Helper::makeSemaphore( GOAP::Helper::makeEvent(), 0 ) )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Button::~Button()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::setBlock( bool _block )
    {
        m_semaphoreBlock->setValue( _block == true ? 1 : 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Button::isBlock() const
    {
        int32_t value = m_semaphoreBlock->getValue();

        return value == 1 ? true : false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::setPickerable( const PickerablePtr & _pickerable, bool _attach )
    {
        m_pickerable = _pickerable;

        if( _attach == true )
        {
            this->addChild( _pickerable );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const PickerablePtr & Button::getPickerable() const
    {
        return m_pickerable;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::setStateNode( EButtonState _state, const NodePtr & _node, bool _attach )
    {
        m_nodes[_state] = _node;

        if( _attach == true )
        {
            this->addChild( _node );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const NodePtr & Button::getStateNode( EButtonState _state ) const
    {
        return m_nodes[_state];
;    }
    //////////////////////////////////////////////////////////////////////////
    bool Button::_activate()
    {
        const NodePtr & nodeIdle = m_nodes[EBS_IDLE];

        if( nodeIdle == nullptr )
        {
            return false;
        }

        if( m_nodes[EBS_OVER] == nullptr )
        {
            m_nodes[EBS_OVER] = nodeIdle;
        }

        if( m_nodes[EBS_PRESSED] == nullptr )
        {
            m_nodes[EBS_PRESSED] = nodeIdle;
        }

        if( m_nodes[EBS_BLOCK] == nullptr )
        {
            m_nodes[EBS_BLOCK] = nodeIdle;
        }

        if( m_nodes[EBS_APPEAR] == nullptr )
        {
            m_state = EBS_IDLE;
        }

        EngineSourcePtr source = Helper::makeEngineSource();

        auto fn = [this]( const EngineSourcePtr & _source )
        {
            typedef void (Button:: * TF)(const EngineSourcePtr &, const NodePtr &);

            const TF stateFunctions[] =
            {
                &Button::__stateAppear,
                &Button::__stateIdle,
                &Button::__stateOver,
                &Button::__stateEnter,
                &Button::__stateLeave,
                &Button::__stateClick,
                &Button::__statePush,
                &Button::__stateRelease,
                &Button::__stateReleasePlay,
                &Button::__statePressed,
                &Button::__stateBlock,
                &Button::__stateBlockEnter,
                &Button::__stateBlockEnd,
            };

            auto stateFunction = stateFunctions[m_state];

            const NodePtr & node = m_nodes[m_state];

            _source->addScope( this, stateFunction, node );

            return true;
        };

        source->addWhile( fn );

        m_chain = Helper::makeEngineChain( source, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE );

        if( m_chain->run() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_deactivate()
    {
        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__setState( EButtonState _state )
    {
        m_state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateAppear( const EngineSourcePtr & _source, const NodePtr & _nodeAppear )
    {
        if( _nodeAppear == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

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
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Button::__setState, EBS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        source_play->addAnimatablePlayWait( _nodeAppear, _nodeAppear );
        source_play->addFunction( this, &Button::__setState, EBS_IDLE );

        _source->addNodeDisable( _nodeAppear );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateIdle( const EngineSourcePtr & _source, const NodePtr & _nodeIdle )
    {
        _source->addNodeEnable( _nodeIdle );
        _source->addLocalDelay( 0.f );

        auto && [source_over_click, source_over_enter, source_block] = _source->addRace<3>();

        source_over_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Button::__setState, EBS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addNodeDisable( _nodeIdle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateOver( const EngineSourcePtr & _source, const NodePtr & _nodeOver )
    {
        _source->addNodeEnable( _nodeOver );
        _source->addLocalDelay( 0.f );

        auto && [source_over_click, source_over_leave, source_block] = _source->addRace<3>();

        source_over_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_over_leave->addFunction( this, &Button::__setState, EBS_LEAVE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addNodeDisable( _nodeOver );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateEnter( const EngineSourcePtr & _source, const NodePtr & _nodeEnter )
    {
        if( _nodeEnter == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );
            _source->addFunction( this, &Button::__setState, EBS_OVER );
            return;
        }

        _source->addNodeEnable( _nodeEnter );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );

        auto && [source_enter_movie, source_enter_leave, source_enter_click] = _source->addRace<3>();
        source_enter_movie->addAnimatablePlayWait( _nodeEnter, _nodeEnter );
        source_enter_movie->addFunction( this, &Button::__setState, EBS_OVER );

        source_enter_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_enter_leave->addEventable( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        source_enter_leave->addFunction( this, &Button::__setState, EBS_IDLE );

        source_enter_click->addPickerableMouseButton( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_enter_click->addFunction( this, &Button::__setState, EBS_PUSH );

        _source->addNodeDisable( _nodeEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateLeave( const EngineSourcePtr & _source, const NodePtr & _nodeLeave )
    {
        if( _nodeLeave == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeLeave );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );

        auto && [source_leave_movie, source_leave_enter] = _source->addRace<2>();
        source_leave_movie->addAnimatablePlayWait( _nodeLeave, _nodeLeave );
        source_leave_movie->addFunction( this, &Button::__setState, EBS_IDLE );

        source_leave_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_leave_enter->addFunction( this, &Button::__setState, EBS_OVER );

        _source->addNodeDisable( _nodeLeave );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePush( const EngineSourcePtr & _source, const NodePtr & _nodePush )
    {
        if( _nodePush == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );
            _source->addFunction( this, &Button::__setState, EBS_PRESSED );

            return;
        }

        _source->addNodeEnable( _nodePush );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );

        auto && [source_Push_movie, source_Push_leave, source_Pressed_click_Rel] = _source->addRace<3>();
        source_Push_movie->addAnimatablePlayWait( _nodePush, _nodePush );
        source_Push_movie->addFunction( this, &Button::__setState, EBS_PRESSED );

        source_Push_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_Push_leave->addEventable( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        source_Push_leave->addFunction( this, &Button::__setState, EBS_RELEASE );

        source_Pressed_click_Rel->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Button::__setState, EBS_CLICK );

        _source->addNodeDisable( _nodePush );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePressed( const EngineSourcePtr & _source, const NodePtr & _nodePressed )
    {
        if( _nodePressed == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );
            _source->addFunction( this, &Button::__setState, EBS_CLICK );

            return;
        }

        _source->addNodeEnable( _nodePressed );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );

        auto && [source_Pressed_click_Rel, source_Pressed_leave, source_block] = _source->addRace<3>();

        source_Pressed_click_Rel->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Button::__setState, EBS_CLICK );

        source_Pressed_leave->addPickerableMouseLeave( m_pickerable, nullptr );
        source_Pressed_leave->addEventable( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        source_Pressed_leave->addFunction( this, &Button::__setState, EBS_RELEASE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addNodeDisable( _nodePressed );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateRelease( const EngineSourcePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );
            _source->addFunction( this, &Button::__setState, EBS_RELEASE_PLAY );

            return;
        }

        _source->addNodeEnable( _nodeRelease );
        _source->addLocalDelay( 0.f );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );

        auto && [source_Release_movie, source_Release_enter] = _source->addRace<2>();
        source_Release_movie->addPickerableMouseButton( m_pickerable, MC_LBUTTON, false, true, nullptr );
        source_Release_movie->addFunction( this, &Button::__setState, EBS_RELEASE_PLAY );

        source_Release_enter->addPickerableMouseEnter( m_pickerable, nullptr );
        source_Release_enter->addFunction( this, &Button::__setState, EBS_PRESSED );

        _source->addNodeDisable( _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateReleasePlay( const EngineSourcePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeRelease );
        _source->addAnimatablePlayWait( _nodeRelease, _nodeRelease );
        _source->addAnimatableRewind( _nodeRelease );
        _source->addFunction( this, &Button::__setState, EBS_IDLE );
        _source->addNodeDisable( _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateClick( const EngineSourcePtr & _source, const NodePtr & _nodeClick )
    {
        if( _nodeClick == nullptr )
        {
            _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
            _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
            _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeClick );
        _source->addAnimatablePlayWait( _nodeClick, _nodeClick );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
        _source->addEventable( this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

        _source->addFunction( this, &Button::__setState, EBS_IDLE );
        _source->addNodeDisable( _nodeClick );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnter( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnter )
    {
        if( _nodeBlockEnter == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_BLOCK );

            return;
        }

        _source->addNodeEnable( _nodeBlockEnter );
        _source->addAnimatablePlayWait( _nodeBlockEnter, _nodeBlockEnter );
        _source->addFunction( this, &Button::__setState, EBS_BLOCK );

        _source->addNodeDisable( _nodeBlockEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlock( const EngineSourcePtr & _source, const NodePtr & _nodeBlock )
    {
        if( _nodeBlock == nullptr )
        {
            _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
            _source->addFunction( this, &Button::__setState, EBS_BLOCK_END );

            return;
        }

        _source->addNodeEnable( _nodeBlock );

        _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
        _source->addFunction( this, &Button::__setState, EBS_BLOCK_END );

        _source->addNodeDisable( _nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnd( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnd )
    {
        if( _nodeBlockEnd == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addNodeEnable( _nodeBlockEnd );

        _source->addAnimatablePlayWait( _nodeBlockEnd, _nodeBlockEnd );
        _source->addFunction( this, &Button::__setState, EBS_IDLE );

        _source->addNodeDisable( _nodeBlockEnd );
    }
}