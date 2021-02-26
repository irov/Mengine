#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/PixelFormat.h"
#include "Kernel/Document.h"

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
        virtual void getImageDesc( RenderImageDesc * const _desc ) const = 0;

    public:
        virtual bool load( const RenderImageInterfacePtr & _image ) const = 0;

    public:
        virtual MemoryInterfacePtr getMemory( uint32_t _codecFlags, const DocumentPtr & _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageLoaderInterface> RenderImageLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
