#include "BitmapFontPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

#include "BitmapGlyphService.h"
#include "BitmapFontConfigLoader.h"
#include "BitmapFontPrototypeGenerator.h"
#include "BitmapFontValidator.h"

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
        SERVICE_CREATE( BitmapGlyphService, MENGINE_DOCUMENT_FACTORABLE );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" )
                , Helper::makeFactorableUnique<BitmapFontPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE )
            );

        VOCABULARY_SET( TextFontConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), Helper::makeFactorableUnique<BitmapFontConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( FontValidatorInterface, STRINGIZE_STRING_LOCAL( "FontValidator" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), Helper::makeFactorableUnique<BitmapFontValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "TextFontConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontValidator" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );

        SERVICE_FINALIZE( BitmapGlyphService );
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( BitmapGlyphService );
    }
    //////////////////////////////////////////////////////////////////////////
}
