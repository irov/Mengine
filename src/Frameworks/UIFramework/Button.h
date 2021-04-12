#pragma once

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/BaseTransformation.h"
#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EButtonState
    {
        EBS_APPEAR,
        EBS_IDLE,
        EBS_OVER,
        EBS_ENTER,
        EBS_LEAVE,
        EBS_CLICK,
        EBS_PUSH,
        EBS_RELEASE,
        EBS_RELEASE_PLAY,
        EBS_PRESSED,
        EBS_BLOCK,
        EBS_BLOCK_ENTER,
        EBS_BLOCK_END,
        __EBS_MAX__,
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void()> LambdaButtonVoidCb;
    //////////////////////////////////////////////////////////////////////////
    class Button
        : public Node
        , protected BaseEventation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Button );
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Button();
        ~Button() override;

    public:
        void setBlock( bool _block );
        bool isBlock() const;

    public:
        void setPickerable( const PickerablePtr & _pickerable, bool _attach );
        const PickerablePtr & getPickerable() const;

    public:
        void setStateNode( EButtonState _state, const NodePtr & _node, bool _attach );
        const NodePtr & getStateNode( EButtonState _state ) const;

    public:
        void setCursorEnterCb( const LambdaButtonVoidCb & _cb );
        const LambdaButtonVoidCb & getCursorEnterCb() const;

        void setCursorLeaveCb( const LambdaButtonVoidCb & _cb );
        const LambdaButtonVoidCb & getCursorLeaveCb() const;

    protected:
        void _dispose() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void _onCursorEnter() const;
        void _onCursorLeave() const;
        void _updateCursorLeave() const;

    protected:
        void __setState( EButtonState _state );

    protected:
        void __stateAppear( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeAppear );
        void __stateIdle( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeIdle );
        void __stateOver( const GOAP::SourceInterfacePtr & source, const NodePtr & _nodeOver );
        void __stateEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeEnter );
        void __stateLeave( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeLeave );
        void __statePush( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePush );
        void __statePressed( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePressed );
        void __stateRelease( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease );
        void __stateReleasePlay( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease );
        void __stateClick( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeClick );
        void __stateBlockEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnter );
        void __stateBlock( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlock );
        void __stateBlockEnd( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnd );

    protected:
        EButtonState m_state;

        PickerablePtr m_pickerable;
        NodePtr m_nodes[__EBS_MAX__];

        GOAP::ChainInterfacePtr m_chain;
        GOAP::SemaphoreInterfacePtr m_semaphoreBlock;

        LambdaButtonVoidCb m_cursorEnterCb;
        LambdaButtonVoidCb m_cursorLeaveCb;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Button> ButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}