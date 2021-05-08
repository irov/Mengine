#pragma once

#include "Interface/RenderImageLoaderInterface.h"

#include "Kernel/Document.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageProviderInterface
        : public Mixin
    {
    public:
        virtual RenderImageLoaderInterfacePtr getLoader( const DocumentPtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageProviderInterface> RenderImageProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
