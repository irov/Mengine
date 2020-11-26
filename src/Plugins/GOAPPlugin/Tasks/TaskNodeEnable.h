#pragma once

#include "Kernel/Node.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskNodeEnable
        : public GOAP::TaskInterface
    {
    public:
        TaskNodeEnable( GOAP::Allocator * _allocator, const NodePtr & _node );
        ~TaskNodeEnable() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        NodePtr m_node;
    };
}