#include "BitmapFontPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

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
    BitmapFontPlugin::~BitmapFontPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontPlugin::_initializePlugin()
    {
        SERVICE_CREATE( BitmapGlyphService );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" )
                , Helper::makeFactorableUnique<BitmapFontPrototypeGenerator>()
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );

        SERVICE_FINALIZE( BitmapGlyphService );
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( BitmapGlyphService );
    }
}
