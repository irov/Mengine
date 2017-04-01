#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "Core/ServantBase.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
    class DecoderRenderImageLoader
        : public ServantBase<RenderImageLoaderInterface>
    {
    public:
        DecoderRenderImageLoader();
        ~DecoderRenderImageLoader();

    public:
        bool initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName );

    protected:
        RenderImageDesc getImageDesc() const override;

    protected:
        bool load( void * _buffer, uint32_t _pitch ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName ) const;
        
    protected:
        ImageDecoderInterfacePtr m_decoder;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<DecoderRenderImageLoader> DecoderRenderImageLoaderPtr;
    //////////////////////////////////////////////////////////////////////////
}	// namespace Menge
