#	include "BitmapFontPlugin.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#	include "Logger/Logger.h"

#	include "BitmapGlyphService.h"
#	include "BitmapFontPrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( BitmapGlyphService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( BitmapFont, Menge::BitmapFontPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	BitmapFontPlugin::BitmapFontPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFontPlugin::_initialize()
	{
		SERVICE_CREATE( m_serviceProvider, BitmapGlyphService );

		PROTOTYPE_SERVICE( m_serviceProvider )
			->addPrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Bitmap" )
				, new FactorableUnique<BitmapFontPrototypeGenerator>()
			);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BitmapFontPlugin::_finalize()
	{
		PROTOTYPE_SERVICE( m_serviceProvider )
			->removePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Font" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "Bitmap" ) );

		SERVICE_FINALIZE( m_serviceProvider, BitmapGlyphServiceInterface );
	}
}
