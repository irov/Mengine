#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Servant.h"
#	include "Config/String.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class TextureGlyphProviderInterface
    {
    public:
        virtual void onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	class TTFAtlasServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TTFAtlasService" )

	public:
		virtual RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _channel, TextureGlyphProviderInterface * _provider, mt::uv4f & _uv ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	struct TTFGlyph
	{
		uint32_t ch;

		float advance;

		float dx;
		float dy;
		float w;
		float h;

		mt::uv4f uv;
		RenderTextureInterfacePtr texture;
	};
	//////////////////////////////////////////////////////////////////////////
	class TTFFontInterface
		: public Servant
	{
	public:
		virtual U32String prepareText( const String & _text ) = 0;
		virtual const TTFGlyph * getGlyph( uint32_t _ch ) const = 0;
        virtual float getKerning( uint32_t _lch, uint32_t _rch ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TTFFontInterface> TTFFontInterfacePtr;
}

#   define TTFATLAS_SERVICE( serviceProvider )\
    ((Menge::TTFAtlasServiceInterface *)SERVICE_GET(serviceProvider, Menge::TTFAtlasServiceInterface))

#   define TTFFONT_SERVICE( serviceProvider )\
    ((Menge::TTFFontManagerInterface *)SERVICE_GET(serviceProvider, Menge::TTFFontManagerInterface))
