#pragma once

#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderImageProviderInterface.h"
#include "Interface/RenderImageLockedInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"
#include "Kernel/Rect.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/Unknowable.h"
#include "Kernel/UnknownPointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderImageInterface
        : public Unknowable
    {
    public:
        virtual void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) = 0;
        virtual const RenderImageProviderInterfacePtr & getRenderImageProvider() const = 0;

    public:
        virtual uint32_t getHWMipmaps() const = 0;
        virtual uint32_t getHWWidth() const = 0;
        virtual uint32_t getHWHeight() const = 0;
        virtual uint32_t getHWChannels() const = 0;
        virtual uint32_t getHWDepth() const = 0;

        virtual EPixelFormat getHWPixelFormat() const = 0;

    public:
        virtual float getHWWidthInv() const = 0;
        virtual float getHWHeightInv() const = 0;

    public:
        virtual RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly = true ) = 0;
        virtual bool unlock( const RenderImageLockedInterfacePtr & _lock, uint32_t _level, bool _successful ) = 0;

    public:
        virtual UnknownPointer getRenderImageExtention() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageInterface> RenderImageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
