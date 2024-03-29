#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/Viewport.h"

#include "math/vec2.h"
#include "math/uv4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTargetInterface
        : public Mixin
    {
    public:
        virtual uint32_t getHWMipmaps() const = 0;
        virtual uint32_t getHWWidth() const = 0;
        virtual uint32_t getHWHeight() const = 0;

    public:
        virtual EPixelFormat getHWPixelFormat() const = 0;

    public:
        virtual float getHWWidthInv() const = 0;
        virtual float getHWHeightInv() const = 0;

    public:
        virtual bool getUpscalePow2() const = 0;

    public:
        virtual bool begin() const = 0;
        virtual void end() const = 0;

    public:
        virtual const mt::uv4f & getUV() const = 0;

    public:
        virtual bool getData( void * const _buffer, size_t _pitch ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTargetInterface> RenderTargetInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}