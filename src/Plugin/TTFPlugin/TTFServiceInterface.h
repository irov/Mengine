#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Servant.h"
#	include "Config/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class TTFAtlasServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TTFAtlasService" )

	public:
		virtual RenderTextureInterfacePtr makeTextureGlyph( uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, mt::uv4f & _uv ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	struct TTFGlyph
	{
		uint32_t ch;

		float advance;

		float dx;
		float dy;
		float ax;
		float ay;

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
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TTFFontInterface> TTFFontInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class TTFFontManagerInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TTFFontManager" );

	public:
		virtual bool createFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName ) = 0;
		virtual TTFFontInterfacePtr getFont( const ConstString & _name ) const = 0;
	};
}

#   define TTFATLAS_SERVICE( serviceProvider )\
    ((Menge::TTFAtlasServiceInterface *)SERVICE_GET(serviceProvider, Menge::TTFAtlasServiceInterface))

#   define TTFFONT_SERVICE( serviceProvider )\
    ((Menge::TTFFontManagerInterface *)SERVICE_GET(serviceProvider, Menge::TTFFontManagerInterface))
