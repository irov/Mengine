#include "BitmapFontConfigLoader.h"

#include "Interface/PrototypeServiceInterface.h"

#include "BitmapFontInterface.h"

#include "BitmapFont.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BitmapFontConfigLoader::BitmapFontConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BitmapFontConfigLoader::~BitmapFontConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BitmapFontConfigLoader::load( const TextFontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config )
    {
        BitmapFontPtr font = _font;

        const ConstString & name = _font->getName();

        Color colorFont;
        if( _config->hasValue( name.c_str(), "ColorFont", &colorFont ) == true )
        {
            _font->setFontColor( colorFont );
        }

        float lineOffset;
        if( _config->hasValue( name.c_str(), "LineOffset", &lineOffset ) == true )
        {
            _font->setLineOffset( lineOffset );
        }

        float charOffset;
        if( _config->hasValue( name.c_str(), "CharOffset", &charOffset ) == true )
        {
            _font->setCharOffset( charOffset );
        }

        FilePath glyphPath;
        if( _config->hasValue( name.c_str(), "Glyph", &glyphPath ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' don't setup Glyph"
                , name.c_str()
            );

            return false;
        }

        BitmapGlyphPtr glyph = BITMAPGLYPH_SERVICE()
            ->getGlyph( _fileGroup, glyphPath );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, "invalid font '%s' don't load Glyph '%s'"
            , name.c_str()
            , glyphPath.c_str()
        );

        font->setBitmapGlyph( glyph );

        uint32_t height = glyph->getHeight();

        font->setHeight( height );

        FilePath pathFontImage;
        if( _config->hasValue( name.c_str(), "Image", &pathFontImage ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' dont setup Image"
                , name.c_str()
            );

            return false;
        }

        ContentInterfacePtr content = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        content->setFileGroup( _fileGroup );
        content->setFilePath( pathFontImage );

        font->setContent( content );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}