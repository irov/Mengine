#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Kernel/Servant.h"
#	include "Config/String.h"

#   include "math/uv4.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class TextureGlyphProviderInterface
    {
    public:
        virtual bool onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) = 0;
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

#   define TTFATLAS_SERVICE()\
    ((Menge::TTFAtlasServiceInterface *)SERVICE_GET(Menge::TTFAtlasServiceInterface))

#   define TTFFONT_SERVICE()\
    ((Menge::TTFFontManagerInterface *)SERVICE_GET(Menge::TTFFontManagerInterface))
