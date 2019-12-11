#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownVideoSurfaceInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceVideo( const ResourcePtr & _resourceVideo ) = 0;
        virtual const ResourcePtr & getResourceVideo() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownVideoSurfaceInterface> UnknownVideoSurfaceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
