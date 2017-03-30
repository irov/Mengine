#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ImageCodecInterface.h"

#   include "Core/ServantBase.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
	class DecoderRenderImageProviderInterface
		: public ServantBase<RenderImageProviderInterface>
	{
	public:
        DecoderRenderImageProviderInterface();
		~DecoderRenderImageProviderInterface();

    public:
        void initialize( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecName );

    protected:
        MemoryInterfacePtr getImageMemory( uint32_t & _mipmaps, uint32_t & _width, uint32_t & _height, uint32_t & _channels, uint32_t & _depth, PixelFormat & _format ) const override;

    protected:
        ImageDecoderInterfacePtr createImageDecoder_() const;

	protected:        
        ConstString m_pakName;
        FilePath m_fileName;
        ConstString m_codecName;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DecoderRenderImageProviderInterface> DecoderRenderImageProviderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
