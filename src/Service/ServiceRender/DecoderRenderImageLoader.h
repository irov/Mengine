#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/ImageCodecInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Core/ServantBase.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

namespace Mengine
{
    class DecoderRenderImageLoader
        : public ServantBase<RenderImageLoaderInterface>
    {
    public:
        DecoderRenderImageLoader();
        ~DecoderRenderImageLoader() override;

    public:
        bool initialize( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _codecName );

    protected:
        RenderImageDesc getImageDesc() const override;

    protected:
        bool load( const RenderImageInterfacePtr & _image ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_( const FileGroupInterfacePtr& _fileGroup, const FilePath & _fileName, const ConstString & _codecName ) const;
        
    protected:
        ImageDecoderInterfacePtr m_decoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DecoderRenderImageLoader> DecoderRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}	
