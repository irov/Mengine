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
		: m_library( nullptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFPlugin::_initialize()
	{
		SERVICE_CREATE( m_serviceProvider, TTFAtlasService );

		if( FT_Init_FreeType( &m_library ) )
		{
			return false;
		}

		TTFPrototypeGeneratorPtr generator = new FactorableUnique<TTFPrototypeGenerator>();

		generator->setFTLibrary( m_library );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "TTF" )
				, new FactorableUnique<TTFPrototypeGenerator>()
			);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFPlugin::_finalize()
	{
		PROTOTYPE_SERVICE( m_serviceProvider )
			->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "TTF" ) );

		SERVICE_FINALIZE( m_serviceProvider, TTFAtlasServiceInterface );
	}
}
