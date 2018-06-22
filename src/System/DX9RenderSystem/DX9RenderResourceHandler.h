#pragma once

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderResourceHandlerInterface
        : public stdex::intrusive_linked<RenderResourceHandlerInterface>
    {
    public:
        virtual void onRenderReset() = 0;
        virtual void onRenderRestore() = 0;
    };
}