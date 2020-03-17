#pragma once

#include "Kernel/Node.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskNodeEnable
        : public GOAP::Task
    {
    public:
        explicit TaskNodeEnable( const NodePtr & _node );
        ~TaskNodeEnable() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        NodePtr m_node;
    };
}