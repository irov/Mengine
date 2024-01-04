#include "BitmapFontConfigLoader.h"

#include "Interface/FontServiceInterface.h"
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
    bool BitmapFontConfigLoader::load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _fileGroup );
        MENGINE_UNUSED( _doc );

        BitmapFontPtr font = BitmapFontPtr::from( _factorable );

        const ConstString & name = font->getName();

        Color colorFont;
        if( _config->hasValue( name.c_str(), "ColorFont", Color(), &colorFont ) == true )
        {
            font->setFontColor( colorFont );
        }

        float lineOffset;
        if( _config->hasValue( name.c_str(), "LineOffset", 0.f, &lineOffset ) == true )
        {
            font->setLineOffset( lineOffset );
        }

        float charOffset;
        if( _config->hasValue( name.c_str(), "CharOffset", 0.f, &charOffset ) == true )
        {
            font->setCharOffset( charOffset );
        }

        ConstString glyphName;
        if( _config->hasValue( name.c_str(), "Glyph", FilePath::none(), &glyphName ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' don't setup Glyph"
                , name.c_str()
            );

            return false;
        }

        const BitmapFontGlyphPtr & glyph = FONT_SERVICE()
            ->getGlyph( glyphName );

        MENGINE_ASSERTION_MEMORY_PANIC( glyph, "invalid font '%s' don't load glyph '%s'"
            , name.c_str()
            , glyphName.c_str()
        );

        font->setBitmapFontGlyph( glyph );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}