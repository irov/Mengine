#pragma once

#include "Kernel/Node.h"

#include "GOAP/Task.h"

namespace Mengine
{
    class TaskNodeDestroy
        : public GOAP::Task
    {
    public:
        explicit TaskNodeDestroy( const NodePtr & _node );
        ~TaskNodeDestroy() override;

    protected:
        bool _onRun() override;
        void _onFinally() override;

    protected:
        NodePtr m_node;
    };
}