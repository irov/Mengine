#	include "TTFPlugin.h"

#	include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Core/PixelFormat.h"

#	include "Logger/Logger.h"

#	include "TTFPrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( TTFAtlasService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TTF, Menge::TTFPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFPlugin::TTFPlugin()
		: m_ftlibrary( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFPlugin::_initialize()
	{
		SERVICE_CREATE( m_serviceProvider, TTFAtlasService );

		FT_Library ftlibrary;
		FT_Error ft_err = FT_Init_FreeType( &ftlibrary );

		if( ft_err != 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("TTFPlugin::_initialize invalid init FreeType '%d'"
				, ft_err
				);

			return false;
		}

		TTFPrototypeGeneratorPtr generator = new FactorableUnique<TTFPrototypeGenerator>();

		generator->setFTLibrary( ftlibrary );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "TTF" )
				, generator
			);

		m_ftlibrary = ftlibrary;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFPlugin::_finalize()
	{
		PROTOTYPE_SERVICE( m_serviceProvider )
			->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "TTF" ) );

		SERVICE_FINALIZE( m_serviceProvider, TTFAtlasServiceInterface );

		FT_Done_FreeType( m_ftlibrary );
		m_ftlibrary = nullptr;
	}
}
