#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class DecoderRenderImageLoader
        : public RenderImageLoaderInterface
        , public Factorable
    {
    public:
        DecoderRenderImageLoader();
        ~DecoderRenderImageLoader() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType, uint32_t _codecFlags );

    protected:
        void getImageDesc( RenderImageDesc * _desc ) const override;

    protected:
        bool load( const RenderImageInterfacePtr & _image ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _codecType ) const;

    protected:
        ImageDecoderInterfacePtr m_decoder;
        uint32_t m_codecFlags;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageLoader> DecoderRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}
