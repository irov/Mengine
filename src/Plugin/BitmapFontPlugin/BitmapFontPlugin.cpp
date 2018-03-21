#include "BitmapFontPlugin.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#include "Logger/Logger.h"

#include "BitmapGlyphService.h"
#include "BitmapFontPrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( BitmapGlyphService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( BitmapFont, Mengine::BitmapFontPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	BitmapFontPlugin::BitmapFontPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool BitmapFontPlugin::_initialize()
	{
		SERVICE_CREATE( BitmapGlyphService );

		PROTOTYPE_SERVICE()
			->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" )
				, new FactorableUnique<BitmapFontPrototypeGenerator>()
			);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void BitmapFontPlugin::_finalize()
	{
		PROTOTYPE_SERVICE()
			->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );

		SERVICE_FINALIZE( BitmapGlyphServiceInterface );
	}
}
