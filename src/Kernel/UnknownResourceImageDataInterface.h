#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceImageDataInterface
        : public UnknownInterface
    {
    public:
        virtual uint32_t getImageWidth() const = 0;
        virtual uint32_t getImageHeight() const = 0;

        virtual Pointer getImageBuffer() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownResourceImageDataInterface> UnknownResourceImageDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}