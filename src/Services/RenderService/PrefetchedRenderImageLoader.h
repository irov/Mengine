#pragma once

#include "Interface/RenderImageLoaderInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrefetchedRenderImageLoader
        : public RenderImageLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PrefetchedRenderImageLoader );

    public:
        PrefetchedRenderImageLoader();
        ~PrefetchedRenderImageLoader() override;

    public:
        void initialize( const RenderImageDesc & _desc, const MemoryBufferInterfacePtr & _pixels, uint32_t _codecFlags );

    public:
        void getImageDesc( RenderImageDesc * const _desc ) const override;
        MemoryInterfacePtr getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const override;
        bool load( const RenderImageInterfacePtr & _image ) const override;

    protected:
        RenderImageDesc m_desc;
        MemoryBufferInterfacePtr m_pixels;
        uint32_t m_codecFlags;
        size_t m_rowBytes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrefetchedRenderImageLoader, RenderImageLoaderInterface> PrefetchedRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
