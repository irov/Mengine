#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DecoderRenderImageLoader
        : public RenderImageLoaderInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DecoderRenderImageLoader );

    public:
        DecoderRenderImageLoader();
        ~DecoderRenderImageLoader() override;

    public:
        bool initialize( const ContentInterfacePtr & _content, uint32_t _codecFlags );
        void finalize();

    protected:
        void getImageDesc( RenderImageDesc * const _desc ) const override;

    protected:
        bool load( const RenderImageInterfacePtr & _image ) const override;

    protected:
        MemoryInterfacePtr getMemory( uint32_t _codecFlags, const DocumentInterfacePtr & _doc ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_( const ContentInterfacePtr & _content ) const;

    protected:
        ImageDecoderInterfacePtr m_decoder;
        uint32_t m_codecFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageLoader, RenderImageLoaderInterface> DecoderRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
