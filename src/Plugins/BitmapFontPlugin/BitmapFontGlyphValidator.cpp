#include "BitmapFontGlyphValidator.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphValidator::validate( const FactorablePtr & _factorable ) const
    {
        BitmapFontGlyphPtr fontGlyph = BitmapFontGlyphPtr::from( _factorable );

        const ContentInterfacePtr & imageContent = fontGlyph->getBitmapImageContent();

        if( imageContent->exist( true ) == false )
        {
            LOGGER_ERROR( "bitmap font glyph '%s' not found image file '%s'"
                , fontGlyph->getName().c_str()
                , Helper::getContentFullPath( imageContent )
            );

            return false;
        }

        const ContentInterfacePtr & licenseContent = fontGlyph->getBitmapLicenseContent();

        if( licenseContent->exist( true ) == false )
        {
            LOGGER_ERROR( "bitmap font glyph '%s' not found license file '%s'"
                , fontGlyph->getName().c_str()
                , Helper::getContentFullPath( licenseContent )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}