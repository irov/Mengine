#include "BitmapFontPlugin.h"

#include "Interface/PrototypeServiceInterface.h"

#include "BitmapFont.h"
#include "BitmapFontService.h"
#include "BitmapFontConfigLoader.h"
#include "BitmapFontGlyphConfigLoader.h"
#include "BitmapFontPrototypeGenerator.h"
#include "BitmapFontGlyphPrototypeGenerator.h"
#include "BitmapFontValidator.h"
#include "BitmapFontGlyphValidator.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( BitmapFontService );
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
        if( SERVICE_CREATE( BitmapFontService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" )
                , Helper::makeFactorableUnique<BitmapFontPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE )
            );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "FontGlyph" ), STRINGIZE_STRING_LOCAL( "Bitmap" )
                , Helper::makeFactorableUnique<BitmapFontGlyphPrototypeGenerator>( MENGINE_DOCUMENT_FACTORABLE )
            );

        VOCABULARY_SET( ConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), Helper::makeFactorableUnique<BitmapFontConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ConfigLoaderInterface, STRINGIZE_STRING_LOCAL( "FontGlyphConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), Helper::makeFactorableUnique<BitmapFontGlyphConfigLoader>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "FontValidator" ), BitmapFont::getFactorableType(), Helper::makeFactorableUnique<BitmapFontValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( ValidatorInterface, STRINGIZE_STRING_LOCAL( "FontGlyphValidator" ), BitmapFont::getFactorableType(), Helper::makeFactorableUnique<BitmapFontGlyphValidator>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Font" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "FontGlyph" ), STRINGIZE_STRING_LOCAL( "Bitmap" ), nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontGlyphConfigLoader" ), STRINGIZE_STRING_LOCAL( "Bitmap" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontValidator" ), BitmapFont::getFactorableType() );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FontGlyphValidator" ), BitmapFont::getFactorableType() );

        SERVICE_FINALIZE( BitmapFontService );
    }
    //////////////////////////////////////////////////////////////////////////
    void BitmapFontPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( BitmapFontService );
    }
    //////////////////////////////////////////////////////////////////////////
}
