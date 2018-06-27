#pragma once

#include "Interface/ServantInterface.h"

#include "Core/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTargetInterface
        : public ServantInterface
    {
    public:
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual uint32_t getChannels() const = 0;
        virtual uint32_t getDepth() const = 0;
        virtual PixelFormat getPixelFormat() const = 0;

    public:
        virtual uint32_t getHWWidth() const = 0;
        virtual uint32_t getHWHeight() const = 0;

    public:
        virtual float getHWWidthInv() const = 0;
        virtual float getHWHeightInv() const = 0;

    public:
        virtual bool begin() = 0;
        virtual void end() = 0;

    public:
        virtual bool getData( void * _buffer, size_t _pitch ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTargetInterface> RenderTargetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}