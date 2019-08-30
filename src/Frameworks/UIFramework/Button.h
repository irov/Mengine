#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    class Button
        : public Node
    {
        DECLARE_VISITABLE( Node );

    public:
        Button();
        ~Button() override;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        NodePtr m_nodeIdle;
        NodePtr m_nodeAppear;
        NodePtr m_nodeEnter;
        NodePtr m_nodeOver;
        NodePtr m_nodeClick;
        NodePtr m_nodeLeave;

        NodePtr m_nodePush;
        NodePtr m_nodePressed;
        NodePtr m_nodeRelease;

        NodePtr m_nodeBlock;
        NodePtr m_nodeBlockEnter;
        NodePtr m_nodeBlockEnd;
    };
}