#pragma once

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderResourceHandler
        : public stdex::intrusive_linked<DX9RenderResourceHandler>
    {
    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;
    };
}