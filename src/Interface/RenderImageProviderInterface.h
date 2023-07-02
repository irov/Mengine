#pragma once

#include "Interface/RenderImageLoaderInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageProviderInterface
        : public Mixin
    {
    public:
        virtual RenderImageLoaderInterfacePtr getLoader( const DocumentInterfacePtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageProviderInterface> RenderImageProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
