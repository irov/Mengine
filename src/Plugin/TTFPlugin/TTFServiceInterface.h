#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Servant.h"

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
	class TTFFontInterface
		: public Servant
	{

	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<TTFFontInterface> TTFFontInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class TTFFontManagerInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TTFFontManager" );

	public:
		virtual TTFFontInterfacePtr createFont( const ConstString & _name, const ConstString& _pakName, const FilePath& _fileName ) = 0;
		virtual TTFFontInterfacePtr getFont( const ConstString & _name ) const = 0;
	};
}

#   define TTFATLAS_SERVICE( serviceProvider )\
    ((Menge::TTFAtlasServiceInterface *)SERVICE_GET(serviceProvider, Menge::TTFAtlasServiceInterface))
