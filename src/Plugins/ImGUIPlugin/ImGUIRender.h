#pragma once

#include "ImGUIInterface.h"

#include "Kernel/Node.h"

namespace Mengine
{
    class ImGUIRender
        : public Node
    {
        DECLARE_VISITABLE( Node );

    public:
        ImGUIRender();
        ~ImGUIRender() override;

    protected:
        bool _activate() override;
        void _deactivate() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ImGUIRender> ImGUIPtr;
    //////////////////////////////////////////////////////////////////////////
}