#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/RenderContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderInterface;
    //////////////////////////////////////////////////////////////////////////
    class RenderOrderInterface
        : public ServantInterface
    {
    public:
        virtual void addRender( int32_t _index, RenderInterface * _render ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderOrderInterface> RenderOrderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}