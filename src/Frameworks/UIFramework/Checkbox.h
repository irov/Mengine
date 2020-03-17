#pragma once

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseEventation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ECheckboxState
    {
        ECS_APPEAR,
        ECS_IDLE,
        ECS_OVER,
        ECS_ENTER,        
        ECS_LEAVE,
        ECS_CLICK,
        ECS_PUSH,
        ECS_RELEASE,
        ECS_PRESSED,
        ECS_BLOCK,
        ECS_BLOCK_ENTER,
        ECS_BLOCK_END,

        __ECS_MAX__,
    };
    //////////////////////////////////////////////////////////////////////////
    class Checkbox
        : public Node
        , public BaseEventation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_EVENTABLE();

    public:
        Checkbox();
        ~Checkbox() override;

    public:
        void setValue( bool _value );
        bool getValue() const;

    public:
        void setBlock( bool _block );
        bool isBlock() const;

    public:
        void setPickerable( const PickerablePtr & _pickerable );
        const PickerablePtr & getPickerable() const;

    public:
        void setStateNode( bool _select, ECheckboxState _state, const NodePtr & _node );
        const NodePtr & getStateNode( bool _select, ECheckboxState _state ) const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:        
        void __setState( bool _value, ECheckboxState _state );

    protected:
        void __stateAppear( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeAppear, bool _value );
        void __stateIdle( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeIdle, bool _value );
        void __stateOver( const GOAP::SourceInterfacePtr & source, const NodePtr & _nodeOver, bool _value );
        void __stateEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeEnter, bool _value );
        void __stateLeave( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeLeave, bool _value );
        void __stateClick( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeClick, bool _value );
        void __statePush( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePush, bool _value );
        void __stateRelease( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeRelease, bool _value );
        void __statePressed( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodePressed, bool _value );
        void __stateBlock( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlock, bool _value );
        void __stateBlockEnter( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnter, bool _value );
        void __stateBlockEnd( const GOAP::SourceInterfacePtr & _source, const NodePtr & _nodeBlockEnd, bool _value );

    protected:
        bool m_value;

        ECheckboxState m_state;

        PickerablePtr m_pickerable;
        NodePtr m_nodes[2][__ECS_MAX__];

        GOAP::ChainInterfacePtr m_chain;
        GOAP::SemaphoreInterfacePtr m_semaphoreBlock;
        GOAP::SemaphoreInterfacePtr m_semaphoreValue;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Checkbox> CheckboxPtr;
    //////////////////////////////////////////////////////////////////////////
}