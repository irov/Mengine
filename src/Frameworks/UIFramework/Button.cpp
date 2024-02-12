#include "Button.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "ButtonEventReceiverInterface.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Button::Button()
        : m_state( EBS_APPEAR )
        , m_semaphoreBlock( GOAP_SERVICE()->makeSemaphore( 0 ) )
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

        this->_updateCursorLeave();
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
            NodePtr node = _pickerable;

            this->addChild( node );
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
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::setCursorEnterCb( const LambdaButtonVoidCb & _cb )
    {
        m_cursorEnterCb = _cb;
    }
    //////////////////////////////////////////////////////////////////////////
    const LambdaButtonVoidCb & Button::getCursorEnterCb() const
    {
        return m_cursorEnterCb;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::setCursorLeaveCb( const LambdaButtonVoidCb & _cb )
    {
        m_cursorLeaveCb = _cb;
    }
    //////////////////////////////////////////////////////////////////////////
    const LambdaButtonVoidCb & Button::getCursorLeaveCb() const
    {
        return m_cursorLeaveCb;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_dispose()
    {
        MENGINE_ASSERTION_FATAL( m_chain == nullptr );

        this->_updateCursorLeave();

        m_cursorEnterCb = nullptr;
        m_cursorLeaveCb = nullptr;

        m_pickerable = nullptr;

        for( uint32_t index = 0; index != __EBS_MAX__; ++index )
        {
            m_nodes[index] = nullptr;
        }

        if( m_semaphoreBlock != nullptr )
        {
            m_semaphoreBlock->clearObserverProviders();
            m_semaphoreBlock = nullptr;
        }

        Node::_dispose();
    }
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

        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        auto fn = [this]( const GOAP::SourceInterfacePtr & _source )
        {
            typedef void (Button:: * TF)(const GOAP::SourceInterfacePtr &, const NodePtr &);

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

            Cook::addScope( _source, this, stateFunction, node );

            return true;
        };

        Cook::addWhile( source, fn );

        m_chain = GOAP_SERVICE()
            ->makeChain( source, nullptr, MENGINE_CODE_FILE, MENGINE_CODE_LINE );

        if( m_chain->run() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_deactivate()
    {
        this->_updateCursorLeave();

        if( m_chain != nullptr )
        {
            m_chain->cancel();
            m_chain = nullptr;
        }

        Node::_deactivate();
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_onCursorEnter() const
    {
        if( m_cursorEnterCb == nullptr )
        {
            return;
        }

        m_cursorEnterCb();
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_onCursorLeave() const
    {
        if( m_cursorLeaveCb == nullptr )
        {
            return;
        }

        m_cursorLeaveCb();
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::_updateCursorLeave() const
    {
        bool isBlock = this->isBlock();

        if( isBlock == true && (m_state == EBS_OVER
            || m_state == EBS_ENTER
            || m_state == EBS_CLICK
            || m_state == EBS_PUSH
            || m_state == EBS_PRESSED
            ) )
        {
            this->_onCursorLeave();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__setState( EButtonState _state )
    {
        m_state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateAppear( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeAppear )
    {
        if( _nodeAppear == nullptr )
        {
            Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeAppear );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_true, source_false] = Cook::addIf( _source, [this]()
        {
            return this->isEnable();
        } );

        Cook::addBlock( source_false );

        auto && [source_over_click, source_over_enter, source_block, source_play] = Cook::addRace<4>( _source );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_click, this, &Button::__setState, EBS_PUSH );

        Cook::addPickerableMouseEnter( source_over_enter, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_enter, this, &Button::__setState, EBS_ENTER );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Button::__setState, EBS_BLOCK_ENTER );

        Cook::addAnimatablePlayWait( source_play, _nodeAppear, _nodeAppear, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_play, this, &Button::__setState, EBS_IDLE );

        Cook::addNodeDisable( _source, _nodeAppear );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateIdle( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeIdle )
    {
        Cook::addNodeEnable( _source, _nodeIdle );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_over_click, source_over_enter, source_block] = Cook::addRace<3>( _source );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_click, this, &Button::__setState, EBS_PUSH );

        Cook::addPickerableMouseEnter( source_over_enter, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_enter, this, &Button::__setState, EBS_ENTER );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Button::__setState, EBS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodeIdle );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateOver( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeOver )
    {
        Cook::addNodeEnable( _source, _nodeOver );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );

        auto && [source_over_click, source_over_leave, source_block] = Cook::addRace<3>( _source );

        Cook::addPickerableMouseButton( source_over_click, m_pickerable, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_click, this, &Button::__setState, EBS_PUSH );

        Cook::addPickerableMouseLeave( source_over_leave, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_over_leave, this, &Button::__setState, EBS_LEAVE );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Button::__setState, EBS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodeOver );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeEnter )
    {
        Cook::addFunction( _source, this, &Button::_onCursorEnter );

        if( _nodeEnter == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );
            Cook::addFunction( _source, this, &Button::__setState, EBS_OVER );

            return;
        }

        Cook::addNodeEnable( _source, _nodeEnter );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_ENTER, &ButtonEventReceiverInterface::onButtonMouseEnter );

        auto && [source_enter_movie, source_enter_leave, source_enter_click] = Cook::addRace<3>( _source );
        Cook::addAnimatablePlayWait( source_enter_movie, _nodeEnter, _nodeEnter, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_enter_movie, this, &Button::__setState, EBS_OVER );

        Cook::addPickerableMouseLeave( source_enter_leave, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( source_enter_leave, this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        Cook::addFunction( source_enter_leave, this, &Button::__setState, EBS_IDLE );

        Cook::addPickerableMouseButton( source_enter_click, m_pickerable, MC_LBUTTON, true, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_enter_click, this, &Button::__setState, EBS_PUSH );

        Cook::addNodeDisable( _source, _nodeEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateLeave( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeLeave )
    {
        Cook::addFunction( _source, this, &Button::_onCursorLeave );

        if( _nodeLeave == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
            Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeLeave );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );

        auto && [source_leave_movie, source_leave_enter] = Cook::addRace<2>( _source );
        Cook::addAnimatablePlayWait( source_leave_movie, _nodeLeave, _nodeLeave, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_leave_movie, this, &Button::__setState, EBS_IDLE );

        Cook::addPickerableMouseEnter( source_leave_enter, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_leave_enter, this, &Button::__setState, EBS_ENTER );

        Cook::addNodeDisable( _source, _nodeLeave );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePush( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePush )
    {
        if( _nodePush == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );
            Cook::addFunction( _source, this, &Button::__setState, EBS_PRESSED );

            return;
        }

        Cook::addNodeEnable( _source, _nodePush );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_PUSH, &ButtonEventReceiverInterface::onButtonMousePush );

        auto && [source_Push_movie, source_Push_leave, source_Pressed_click_Rel] = Cook::addRace<3>( _source );
        Cook::addAnimatablePlayWait( source_Push_movie, _nodePush, _nodePush, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_Push_movie, this, &Button::__setState, EBS_PRESSED );

        Cook::addPickerableMouseLeave( source_Push_leave, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( source_Push_leave, this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        Cook::addFunction( source_Push_leave, this, &Button::__setState, EBS_RELEASE );

        Cook::addPickerableMouseButton( source_Pressed_click_Rel, m_pickerable, MC_LBUTTON, false, false, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_Pressed_click_Rel, this, &Button::__setState, EBS_CLICK );

        Cook::addNodeDisable( _source, _nodePush );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__statePressed( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePressed )
    {
        if( _nodePressed == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );
            Cook::addFunction( _source, this, &Button::__setState, EBS_CLICK );

            return;
        }

        Cook::addNodeEnable( _source, _nodePressed );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_PRESSED, &ButtonEventReceiverInterface::onButtonMousePressed );

        auto && [source_Pressed_click_Rel, source_Pressed_leave, source_block] = Cook::addRace<3>( _source );

        Cook::addPickerableMouseButton( source_Pressed_click_Rel, m_pickerable, MC_LBUTTON, false, false, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_Pressed_click_Rel, this, &Button::__setState, EBS_CLICK );

        Cook::addPickerableMouseLeave( source_Pressed_leave, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( source_Pressed_leave, this, EVENT_BUTTON_MOUSE_LEAVE, &ButtonEventReceiverInterface::onButtonMouseLeave );
        Cook::addFunction( source_Pressed_leave, this, &Button::__setState, EBS_RELEASE );

        Cook::addSemaphoreEqual( source_block, m_semaphoreBlock, 1 );
        Cook::addFunction( source_block, this, &Button::__setState, EBS_BLOCK_ENTER );

        Cook::addNodeDisable( _source, _nodePressed );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateRelease( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );
            Cook::addFunction( _source, this, &Button::__setState, EBS_RELEASE_PLAY );

            return;
        }

        Cook::addNodeEnable( _source, _nodeRelease );
        Cook::addLocalDelay( _source, 0.f, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_RELEASE, &ButtonEventReceiverInterface::onButtonMouseRelease );

        auto && [source_Release_movie, source_Release_enter] = Cook::addRace<2>( _source );
        Cook::addPickerableMouseButton( source_Release_movie, m_pickerable, MC_LBUTTON, false, true, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_Release_movie, this, &Button::__setState, EBS_RELEASE_PLAY );

        Cook::addPickerableMouseEnter( source_Release_enter, m_pickerable, nullptr, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( source_Release_enter, this, &Button::__setState, EBS_PRESSED );

        Cook::addNodeDisable( _source, _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateReleasePlay( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease )
    {
        if( _nodeRelease == nullptr )
        {
            Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeRelease );
        Cook::addAnimatablePlayWait( _source, _nodeRelease, _nodeRelease, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addAnimatableRewind( _source, _nodeRelease );
        Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );
        Cook::addNodeDisable( _source, _nodeRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateClick( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeClick )
    {
        if( _nodeClick == nullptr )
        {
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
            Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

            Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeClick );
        Cook::addAnimatablePlayWait( _source, _nodeClick, _nodeClick, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON_BEGIN, &ButtonEventReceiverInterface::onButtonMouseButtonBegin );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON, &ButtonEventReceiverInterface::onButtonMouseButton );
        Cook::addEventable( _source, this, EVENT_BUTTON_MOUSE_BUTTON_END, &ButtonEventReceiverInterface::onButtonMouseButtonEnd );

        Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );
        Cook::addNodeDisable( _source, _nodeClick );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnter )
    {
        if( _nodeBlockEnter == nullptr )
        {
            Cook::addFunction( _source, this, &Button::__setState, EBS_BLOCK );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlockEnter );
        Cook::addAnimatablePlayWait( _source, _nodeBlockEnter, _nodeBlockEnter, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( _source, this, &Button::__setState, EBS_BLOCK );

        Cook::addNodeDisable( _source, _nodeBlockEnter );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlock( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlock )
    {
        if( _nodeBlock == nullptr )
        {
            Cook::addSemaphoreEqual( _source, m_semaphoreBlock, 0 );
            Cook::addFunction( _source, this, &Button::__setState, EBS_BLOCK_END );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlock );

        Cook::addSemaphoreEqual( _source, m_semaphoreBlock, 0 );
        Cook::addFunction( _source, this, &Button::__setState, EBS_BLOCK_END );

        Cook::addNodeDisable( _source, _nodeBlock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Button::__stateBlockEnd( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnd )
    {
        if( _nodeBlockEnd == nullptr )
        {
            Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

            return;
        }

        Cook::addNodeEnable( _source, _nodeBlockEnd );

        Cook::addAnimatablePlayWait( _source, _nodeBlockEnd, _nodeBlockEnd, MENGINE_DOCUMENT_FACTORABLE );
        Cook::addFunction( _source, this, &Button::__setState, EBS_IDLE );

        Cook::addNodeDisable( _source, _nodeBlockEnd );
    }
    //////////////////////////////////////////////////////////////////////////
}