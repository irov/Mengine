#include "TTFFontConfigLoader.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "TTFFont.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontConfigLoader::TTFFontConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontConfigLoader::~TTFFontConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontConfigLoader::load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc )
    {
        TTFFontPtr font = TTFFontPtr::from( _factorable );

        const ConstString & name = font->getName();

        Color colorFont;
        if( _config->hasValue( name.c_str(), "ColorFont", Color(), &colorFont ) == true )
        {
            font->setFontColor( colorFont );
        }

        float lineOffset;
        if( _config->hasValueFloat( name.c_str(), "LineOffset", 0.f, &lineOffset ) == true )
        {
            font->setLineOffset( (float)lineOffset );
        }

        float charOffset;
        if( _config->hasValueFloat( name.c_str(), "CharOffset", 0.f, &charOffset ) == true )
        {
            font->setCharOffset( (float)charOffset );
        }

        ConstString glyphName;
        if( _config->hasValue( name.c_str(), "Glyph", FilePath::none(), &glyphName ) == false )
        {
            LOGGER_ERROR( "bitmap font '%s' don't setup Glyph"
                , name.c_str()
            );

            return false;
        }

        const TTFFontGlyphPtr & glyph = FONT_SERVICE()
            ->getGlyph( glyphName );

        if( glyph == nullptr )
        {
            LOGGER_ERROR( "invalid font '%s' don't found glyph '%s'"
                , name.c_str()
                , glyphName.c_str()
            );

            return false;
        }

        font->setTTFFontGlyph( glyph );

        uint32_t height;
        if( _config->hasValueInteger( name.c_str(), "Height", MENGINE_UINT32_C(0), &height ) == false )
        {
            LOGGER_ERROR( "ttf font '%s' don't setup Height"
                , name.c_str()
            );

            return false;
        }

        font->setHeight( (uint32_t)height );

        FilePath FEPath;
        if( _config->hasValue( name.c_str(), "FEPath", FilePath::none(), &FEPath ) == true )
        {
            ConstString FEType;
            _config->hasValue( name.c_str(), "FEType", STRINGIZE_STRING_LOCAL( "FEFile" ), &FEType );

            FontEffectInterfacePtr fontEffect = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), FEType, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( fontEffect, "ttf font '%s' invalid create font effect '%s'"
                , name.c_str()
                , FEType.c_str()
            );

            ContentInterfacePtr content = Helper::makeFileContent( _fileGroup, FEPath, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid make content '%s'"
                , FEPath.c_str()
            );

            fontEffect->setContent( content );

            ConstString FEName;
            if( _config->hasValue( name.c_str(), "FEName", ConstString::none(), &FEName ) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup FEName"
                    , name.c_str()
                );

                return false;
            }

            UnknownFontEffectFileInterface * unknownFontEffect = fontEffect->getDynamicUnknown();

            MENGINE_ASSERTION_MEMORY_PANIC( unknownFontEffect, "ttf font '%s' font effect '%s' is not file based"
                , name.c_str()
                , FEType.c_str()
            );

            unknownFontEffect->setEffectName( FEName );

            uint32_t FESample;
            if( _config->hasValueInteger( name.c_str(), "FESample", MENGINE_UINT32_C(0), &FESample ) == true )
            {
                fontEffect->setEffectSample( FESample );
            }

            font->setEffect( fontEffect );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}