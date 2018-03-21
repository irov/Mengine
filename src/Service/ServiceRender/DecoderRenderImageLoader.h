#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/ImageCodecInterface.h"

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
        bool initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName );

    protected:
        RenderImageDesc getImageDesc() const override;

    protected:
        bool load( const RenderImageInterfacePtr & _image ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName ) const;
        
    protected:
        ImageDecoderInterfacePtr m_decoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<DecoderRenderImageLoader> DecoderRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}	
