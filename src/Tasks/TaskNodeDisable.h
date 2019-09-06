#pragma once

#include "Kernel/Node.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskNodeDisable
        : public GOAP::Task
    {
    public:
        TaskNodeDisable( const NodePtr & _node );
        ~TaskNodeDisable() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        NodePtr m_node;
    };
}