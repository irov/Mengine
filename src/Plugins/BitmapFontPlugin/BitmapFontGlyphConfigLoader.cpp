#include "BitmapFontGlyphConfigLoader.h"

#include "Interface/CodecServiceInterface.h"

#include "BitmapFontInterface.h"

#include "BitmapFontGlyph.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"

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
    bool BitmapFontGlyphConfigLoader::load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc )
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

        ContentInterfacePtr descriptionContent = Helper::makeFileContent( _fileGroup, descriptionPath, _doc );

        BitmapFontGlyphDescriptionPtr description = BITMAPFONT_SERVICE()
            ->createGlyphDescription( descriptionContent, _doc );

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

        ContentInterfacePtr imageContent = Helper::makeFileContent( _fileGroup, imagePath, _doc );

        const ConstString & imageCodecType = CODEC_SERVICE()
            ->findCodecType( imagePath );

        imageContent->setCodecType( imageCodecType );

        glyph->setBitmapImageContent( imageContent );

        FilePath licensePath;
        if( _config->hasValue( glyphName.c_str(), "License", FilePath::none(), &imagePath ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' dont setup License"
                , glyphName.c_str()
            );

            return false;
        }

        ContentInterfacePtr licenseContent = Helper::makeFileContent( _fileGroup, licensePath, _doc );

        glyph->setBitmapLicenseContent( licenseContent );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}