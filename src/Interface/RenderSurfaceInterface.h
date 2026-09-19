#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderSurfaceInterface
        : public Factorable
    {
    public:
        virtual const Resolution & getResolution() const = 0;
        virtual float getDpiScale() const = 0;

    public:
        virtual bool resize( const Resolution & _resolution, float _dpiScale ) = 0;
        virtual void setVSync( bool _vsync ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderSurfaceInterface> RenderSurfaceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
