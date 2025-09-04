#include "TTFFontConfigLoader.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/FontServiceInterface.h"

#include "Plugins/FEPlugin/FEInterface.h"

#include "TTFFont.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/PrototypeHelper.h"
#include "Kernel/VocabularyHelper.h"

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
        if( _config->hasValueInteger( name.c_str(), "Height", 0U, &height ) == false )
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
            FontEffectInterfacePtr fontEffet = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FEFile" ), _doc );

            ContentInterfacePtr content = Helper::makeFileContent( _fileGroup, FEPath, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid make content '%s'"
                , FEPath.c_str()
            );

            DataflowInterfacePtr dataflowFE = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "feFont" ) );

            content->setDataflow( dataflowFE );

            fontEffet->setContent( content );

            UnknownFEFileInterface * unknownFE = fontEffet->getDynamicUnknown();

            ConstString FEName;
            if( _config->hasValue( name.c_str(), "FEName", ConstString::none(), &FEName ) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup FEName"
                    , name.c_str()
                );

                return false;
            }

            unknownFE->setEffectName( FEName );

            uint32_t FESample;
            if( _config->hasValueInteger( name.c_str(), "FESample", 0U, &FESample ) == true )
            {
                fontEffet->setEffectSample( FESample );
            }

            font->setEffect( fontEffet );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}