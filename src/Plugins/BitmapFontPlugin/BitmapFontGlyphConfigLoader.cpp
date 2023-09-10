#include "BitmapFontGlyphConfigLoader.h"

#include "Interface/PrototypeServiceInterface.h"

#include "BitmapFontInterface.h"

#include "BitmapFontGlyph.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphConfigLoader::BitmapFontGlyphConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontGlyphConfigLoader::~BitmapFontGlyphConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontGlyphConfigLoader::load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config )
    {
        BitmapFontGlyphPtr glyph = BitmapFontGlyphPtr::from( _factorable );

        const ConstString & glyphName = glyph->getName();

        FilePath descriptionPath;
        if( _config->hasValue( glyphName.c_str(), "Description", FilePath::none(), &descriptionPath ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' don't setup GlyphPath"
                , glyphName.c_str()
            );

            return false;
        }

        BitmapFontGlyphDescriptionPtr description = BITMAPFONT_SERVICE()
            ->createGlyphDescription( _fileGroup, descriptionPath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( description, "invalid bitmap font '%s' don't load Symbols '%s'"
            , glyphName.c_str()
            , descriptionPath.c_str()
        );

        glyph->setBitmapFontGlyphDescription( description );

        FilePath imagePath;
        if( _config->hasValue( glyphName.c_str(), "Image", FilePath::none(), &imagePath ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' dont setup Image"
                , glyphName.c_str()
            );

            return false;
        }

        ContentInterfacePtr imageContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        imageContent->setFileGroup( _fileGroup );
        imageContent->setFilePath( imagePath );

        glyph->setBitmapImageContent( imageContent );

        FilePath licensePath;
        if( _config->hasValue( glyphName.c_str(), "License", FilePath::none(), &imagePath ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' dont setup License"
                , glyphName.c_str()
            );

            return false;
        }

        ContentInterfacePtr licenseContent = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        licenseContent->setFileGroup( _fileGroup );
        licenseContent->setFilePath( licensePath );

        glyph->setBitmapLicenseContent( licenseContent );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}