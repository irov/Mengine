#include "Button.h"

#include "Tasks/TaskNodeEnable.h"
#include "Tasks/TaskNodeDisable.h"
#include "Tasks/TaskLocalDelay.h"
#include "Tasks/TaskPickerableMouseButton.h"
#include "Tasks/TaskPickerableMouseEnter.h"
#include "Tasks/TaskPickerableMouseLeave.h"
#include "Tasks/TaskAnimatablePlayWait.h"
#include "Tasks/TaskAnimatableRewind.h"
#include "Tasks/TaskEventable.h"

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

        GOAP::SourcePtr source = GOAP::Helper::makeSource();

        auto fn = [this]( const GOAP::SourcePtr & _source )
        {
            typedef void (Button:: * TF)(const GOAP::SourcePtr &, const NodePtr &);

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

        m_chain = GOAP::Helper::makeChain( source );

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
    void Button::__stateAppear( const GOAP::SourcePtr & _source, const NodePtr & _nodeAppear )
    {
        if( _nodeAppear == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeAppear );
        _source->addTask<TaskLocalDelay>( 0.f );

        auto && [source_true, source_false] = _source->addIf( [this]()
        {
            return this->isEnable();
        } );

        source_false->addBlock();

        auto && [source_over_click, source_over_enter, source_block, source_play] = _source->addRace<4>();

        source_over_click->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_enter->addTask<TaskPickerableMouseEnter>( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Button::__setState, EBS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        source_play->addTask<TaskAnimatablePlayWait>( _nodeAppear, _nodeAppear );
        source_play->addFunction( this, &Button::__setState, EBS_IDLE );

        _source->addTask<TaskNodeDisable>( _nodeAppear );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateIdle( const GOAP::SourcePtr & _source, const NodePtr & _nodeIdle )
    {
        _source->addTask<TaskNodeEnable>( _nodeIdle );
        _source->addTask<TaskLocalDelay>( 0.f );

        auto && [source_over_click, source_over_enter, source_block] = _source->addRace<3>();

        source_over_click->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_enter->addTask<TaskPickerableMouseEnter>( m_pickerable, nullptr );
        source_over_enter->addFunction( this, &Button::__setState, EBS_ENTER );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addTask<TaskNodeDisable>( _nodeIdle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateOver( const GOAP::SourcePtr & _source, const NodePtr & _nodeOver )
    {
        _source->addTask<TaskNodeEnable>( _nodeOver );
        _source->addTask<TaskLocalDelay>( 0.f );

        auto && [source_over_click, source_over_leave, source_block] = _source->addRace<3>();

        source_over_click->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_over_click->addFunction( this, &Button::__setState, EBS_PUSH );

        source_over_leave->addTask<TaskPickerableMouseLeave>( m_pickerable, nullptr );
        source_over_leave->addFunction( this, &Button::__setState, EBS_LEAVE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addTask<TaskNodeDisable>( _nodeOver );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateEnter( const GOAP::SourcePtr & _source, const NodePtr & _nodeEnter )
    {
        if( _nodeEnter == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );
            _source->addFunction( this, &Button::__setState, EBS_OVER );
            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeEnter );
        _source->addTask<TaskLocalDelay>( 0.f );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );

        auto && [source_enter_movie, source_enter_leave, source_enter_click] = _source->addRace<3>();
        source_enter_movie->addTask<TaskAnimatablePlayWait>( _nodeEnter, _nodeEnter );
        source_enter_movie->addFunction( this, &Button::__setState, EBS_OVER );

        source_enter_leave->addTask<TaskPickerableMouseLeave>( m_pickerable, nullptr );
        source_enter_leave->addFunction( this, &Button::__setState, EBS_IDLE );

        source_enter_click->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, true, true, nullptr );
        source_enter_click->addFunction( this, &Button::__setState, EBS_PUSH );

        _source->addTask<TaskNodeDisable>( _nodeEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateLeave( const GOAP::SourcePtr & _source, const NodePtr & _nodeLeave )
    {
        if( _nodeLeave == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeLeave );
        _source->addTask<TaskLocalDelay>( 0.f );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );

        auto && [source_leave_movie, source_leave_enter] = _source->addRace<2>();
        source_leave_movie->addTask<TaskAnimatablePlayWait>( _nodeLeave, _nodeLeave );
        source_leave_movie->addFunction( this, &Button::__setState, EBS_IDLE );

        source_leave_enter->addTask<TaskPickerableMouseEnter>( m_pickerable, nullptr );
        source_leave_enter->addFunction( this, &Button::__setState, EBS_OVER );

        _source->addTask<TaskNodeDisable>( _nodeLeave );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePush( const GOAP::SourcePtr & _source, const NodePtr & _nodePush )
    {
        if( _nodePush == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );
            _source->addFunction( this, &Button::__setState, EBS_PRESSED );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodePush );
        _source->addTask<TaskLocalDelay>( 0.f );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );

        auto && [source_Push_movie, source_Push_leave, source_Pressed_click_Rel] = _source->addRace<3>();
        source_Push_movie->addTask<TaskAnimatablePlayWait>( _nodePush, _nodePush );
        source_Push_movie->addFunction( this, &Button::__setState, EBS_PRESSED );

        source_Push_leave->addTask<TaskPickerableMouseLeave>( m_pickerable, nullptr );
        source_Push_leave->addFunction( this, &Button::__setState, EBS_RELEASE );

        source_Pressed_click_Rel->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Button::__setState, EBS_CLICK );

        _source->addTask<TaskNodeDisable>( _nodePush );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePressed( const GOAP::SourcePtr & _source, const NodePtr & _nodePressed )
    {
        if( _nodePressed == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );
            _source->addFunction( this, &Button::__setState, EBS_CLICK );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodePressed );
        _source->addTask<TaskLocalDelay>( 0.f );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );

        auto && [source_Pressed_click_Rel, source_Pressed_leave, source_block] = _source->addRace<3>();

        source_Pressed_click_Rel->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, false, false, nullptr );
        source_Pressed_click_Rel->addFunction( this, &Button::__setState, EBS_CLICK );

        source_Pressed_leave->addTask<TaskPickerableMouseLeave>( m_pickerable, nullptr );
        source_Pressed_leave->addFunction( this, &Button::__setState, EBS_RELEASE );

        source_block->addSemaphoreEqual( m_semaphoreBlock, 1 );
        source_block->addFunction( this, &Button::__setState, EBS_BLOCK_ENTER );

        _source->addTask<TaskNodeDisable>( _nodePressed );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateRelease( const GOAP::SourcePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );
            _source->addFunction( this, &Button::__setState, EBS_RELEASE_PLAY );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeRelease );
        _source->addTask<TaskLocalDelay>( 0.f );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );

        auto && [source_Release_movie, source_Release_enter] = _source->addRace<2>();
        source_Release_movie->addTask<TaskPickerableMouseButton>( m_pickerable, MC_LBUTTON, false, true, nullptr );
        source_Release_movie->addFunction( this, &Button::__setState, EBS_RELEASE_PLAY );

        source_Release_enter->addTask<TaskPickerableMouseEnter>( m_pickerable, nullptr );
        source_Release_enter->addFunction( this, &Button::__setState, EBS_PRESSED );

        _source->addTask<TaskNodeDisable>( _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateReleasePlay( const GOAP::SourcePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeRelease );
        _source->addTask<TaskAnimatablePlayWait>( _nodeRelease, _nodeRelease );
        _source->addTask<TaskAnimatableRewind>( _nodeRelease );
        _source->addFunction( this, &Button::__setState, EBS_IDLE );
        _source->addTask<TaskNodeDisable>( _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateClick( const GOAP::SourcePtr & _source, const NodePtr & _nodeClick )
    {
        if( _nodeClick == nullptr )
        {
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
            _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeClick );
        _source->addTask<TaskAnimatablePlayWait>( _nodeClick, _nodeClick );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
        _source->addTask<TaskEventable>( this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

        _source->addFunction( this, &Button::__setState, EBS_IDLE );
        _source->addTask<TaskNodeDisable>( _nodeClick );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnter( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlockEnter )
    {
        if( _nodeBlockEnter == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_BLOCK );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeBlockEnter );
        _source->addTask<TaskAnimatablePlayWait>( _nodeBlockEnter, _nodeBlockEnter );
        _source->addFunction( this, &Button::__setState, EBS_BLOCK );

        _source->addTask<TaskNodeDisable>( _nodeBlockEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlock( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlock )
    {
        if( _nodeBlock == nullptr )
        {
            _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
            _source->addFunction( this, &Button::__setState, EBS_BLOCK_END );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeBlock );

        _source->addSemaphoreEqual( m_semaphoreBlock, 0 );
        _source->addFunction( this, &Button::__setState, EBS_BLOCK_END );

        _source->addTask<TaskNodeDisable>( _nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnd( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlockEnd )
    {
        if( _nodeBlockEnd == nullptr )
        {
            _source->addFunction( this, &Button::__setState, EBS_IDLE );

            return;
        }

        _source->addTask<TaskNodeEnable>( _nodeBlockEnd );

        _source->addTask<TaskAnimatablePlayWait>( _nodeBlockEnd, _nodeBlockEnd );
        _source->addFunction( this, &Button::__setState, EBS_IDLE );

        _source->addTask<TaskNodeDisable>( _nodeBlockEnd );
    }
}