#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/RenderPrimitive.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderDrawPrimitiveInterface
        : public Mixin
    {
    public:
        virtual void onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderDrawPrimitiveInterface> RenderDrawPrimitiveInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}