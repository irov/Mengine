#pragma once

#include "Interface/RenderEnumInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"
#include "Kernel/Rect.h"
#include "Kernel/PixelFormat.h"

#include "Config/Typedef.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderImageDesc
    {
        uint32_t mipmaps;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        uint32_t depth;
        EPixelFormat format;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderImageLoaderInterface
        : public Mixin
    {
    public:
        virtual RenderImageDesc getImageDesc() const = 0;

    public:
        virtual bool load( const RenderImageInterfacePtr & _image ) const = 0;
        //virtual bool load( void * _buffer, uint32_t _pitch ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLoaderInterface> RenderImageLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderImageProviderInterface
        : public Mixin
    {
    public:
        virtual RenderImageLoaderInterfacePtr getLoader() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageProviderInterface> RenderImageProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderImageInterface
        : public Mixin
    {
    public:
        virtual void bind( uint32_t _stage ) = 0;

    public:
        virtual void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) = 0;
        virtual const RenderImageProviderInterfacePtr & getRenderImageProvider() const = 0;

    public:
        virtual ERenderImageMode getMode() const = 0;

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
        virtual Pointer lock( size_t * _pitch, uint32_t _level, const Rect& _rect, bool _readOnly = true ) = 0;
        virtual bool unlock( uint32_t _level, bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageInterface> RenderImageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
