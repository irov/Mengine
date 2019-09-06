#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"

#include "GOAP/GOAP.h"

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
    class Button
        : public Node
        , public BaseEventation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();

    public:
        Button();
        ~Button() override;

    public:
        void setBlock( bool _block );
        bool isBlock() const;

    public:
        void setPickerable( const PickerablePtr & _pickerable );
        const PickerablePtr & getPickerable() const;

    public:
        void setStateNode( EButtonState _state, const NodePtr & _node );
        const NodePtr & getStateNode( EButtonState _state ) const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void __setState( EButtonState _state );
        void __stateAppear( const GOAP::SourcePtr & _source, const NodePtr & _nodeAppear );
        void __stateIdle( const GOAP::SourcePtr & _source, const NodePtr & _nodeIdle );
        void __stateOver( const GOAP::SourcePtr & source, const NodePtr & _nodeOver );
        void __stateEnter( const GOAP::SourcePtr & _source, const NodePtr & _nodeEnter );
        void __stateLeave( const GOAP::SourcePtr & _source, const NodePtr & _nodeLeave );
        void __statePush( const GOAP::SourcePtr & _source, const NodePtr & _nodePush );
        void __statePressed( const GOAP::SourcePtr & _source, const NodePtr & _nodePressed );
        void __stateRelease( const GOAP::SourcePtr & _source, const NodePtr & _nodeRelease );
        void __stateReleasePlay( const GOAP::SourcePtr & _source, const NodePtr & _nodeRelease );
        void __stateClick( const GOAP::SourcePtr & _source, const NodePtr & _nodeClick );
        void __stateBlockEnter( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlockEnter );
        void __stateBlock( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlock );
        void __stateBlockEnd( const GOAP::SourcePtr & _source, const NodePtr & _nodeBlockEnd );

    protected:
        EButtonState m_state;

        PickerablePtr m_pickerable;
        NodePtr m_nodes[__EBS_MAX__];

        GOAP::ChainPtr m_chain;
        GOAP::SemaphorePtr m_semaphoreBlock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Button> ButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}