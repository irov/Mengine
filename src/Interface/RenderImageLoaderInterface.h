#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/Mixin.h"
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
        EPixelFormat format;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderImageLoaderInterface
        : public Mixin
    {
    public:
        virtual void getImageDesc( RenderImageDesc * const _desc ) const = 0;

    public:
        virtual bool load( const RenderImageInterfacePtr & _image ) const = 0;

    public:
        virtual MemoryInterfacePtr getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLoaderInterface> RenderImageLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
