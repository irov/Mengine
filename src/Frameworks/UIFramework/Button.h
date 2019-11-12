#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"

#include "Tasks/EngineSource.h"

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
        void setPickerable( const PickerablePtr & _pickerable, bool _attach );
        const PickerablePtr & getPickerable() const;

    public:
        void setStateNode( EButtonState _state, const NodePtr & _node, bool _attach );
        const NodePtr & getStateNode( EButtonState _state ) const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void __setState( EButtonState _state );

    protected:
        void __stateAppear( const EngineSourcePtr & _source, const NodePtr & _nodeAppear );
        void __stateIdle( const EngineSourcePtr & _source, const NodePtr & _nodeIdle );
        void __stateOver( const EngineSourcePtr & source, const NodePtr & _nodeOver );
        void __stateEnter( const EngineSourcePtr & _source, const NodePtr & _nodeEnter );
        void __stateLeave( const EngineSourcePtr & _source, const NodePtr & _nodeLeave );
        void __statePush( const EngineSourcePtr & _source, const NodePtr & _nodePush );
        void __statePressed( const EngineSourcePtr & _source, const NodePtr & _nodePressed );
        void __stateRelease( const EngineSourcePtr & _source, const NodePtr & _nodeRelease );
        void __stateReleasePlay( const EngineSourcePtr & _source, const NodePtr & _nodeRelease );
        void __stateClick( const EngineSourcePtr & _source, const NodePtr & _nodeClick );
        void __stateBlockEnter( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnter );
        void __stateBlock( const EngineSourcePtr & _source, const NodePtr & _nodeBlock );
        void __stateBlockEnd( const EngineSourcePtr & _source, const NodePtr & _nodeBlockEnd );

    protected:
        EButtonState m_state;

        PickerablePtr m_pickerable;
        NodePtr m_nodes[__EBS_MAX__];

        GOAP::ChainInterfacePtr m_chain;
        GOAP::SemaphorePtr m_semaphoreBlock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Button> ButtonPtr;
    //////////////////////////////////////////////////////////////////////////
}