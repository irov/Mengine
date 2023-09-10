#include "BitmapFontGlyphValidator.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphValidator::validate( const FactorablePtr & _factorable ) const
    {
        BitmapFontGlyphPtr fontGlyph = BitmapFontGlyphPtr::from( _factorable );

        const ContentInterfacePtr & imageContent = fontGlyph->getBitmapImageContent();

        if( imageContent->existContent() == false )
        {
            const FileGroupInterfacePtr & fileGroup = imageContent->getFileGroup();
            const FilePath & filePath = imageContent->getFilePath();

            LOGGER_ERROR( "bitmap font glyph '%s' not found image file '%s'"
                , fontGlyph->getName().c_str()
                , Helper::getFileGroupFullPath( fileGroup, filePath )
            );

            return false;
        }

        const ContentInterfacePtr & licenseContent = fontGlyph->getBitmapLicenseContent();

        if( licenseContent->existContent() == false )
        {
            const FileGroupInterfacePtr & fileGroup = licenseContent->getFileGroup();
            const FilePath & filePath = licenseContent->getFilePath();

            LOGGER_ERROR( "bitmap font glyph '%s' not found license file '%s'"
                , fontGlyph->getName().c_str()
                , Helper::getFileGroupFullPath( fileGroup, filePath )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}