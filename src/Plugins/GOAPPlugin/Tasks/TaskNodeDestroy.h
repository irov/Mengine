#pragma once

#include "Kernel/Node.h"

#include "GOAP/TaskInterface.h"

namespace Mengine
{
    class TaskNodeDestroy
        : public GOAP::TaskInterface
    {
    public:
        explicit TaskNodeDestroy( const NodePtr & _node );
        ~TaskNodeDestroy() override;

    protected:
        bool _onRun( GOAP::NodeInterface * _node ) override;
        void _onFinally() override;

    protected:
        NodePtr m_node;
    };
}