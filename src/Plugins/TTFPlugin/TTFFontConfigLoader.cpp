#include "TTFFontConfigLoader.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/FEPlugin/FEInterface.h"

#include "TTFFont.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"

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
    bool TTFFontConfigLoader::load( const FontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config )
    {
        TTFFontPtr font = _font;

        const ConstString & name = font->getName();

        Color colorFont;
        if( _config->hasValue( name.c_str(), "ColorFont", Color(), &colorFont ) == true )
        {
            _font->setFontColor( colorFont );
        }

        float lineOffset;
        if( _config->hasValue( name.c_str(), "LineOffset", 0.f, &lineOffset ) == true )
        {
            _font->setLineOffset( lineOffset );
        }

        float charOffset;
        if( _config->hasValue( name.c_str(), "CharOffset", 0.f, &charOffset ) == true )
        {
            _font->setCharOffset( charOffset );
        }

        bool system;
        _config->hasValue( name.c_str(), "System", false, &system );

        if( system == false )
        {
            FilePath ttfPath;
            if( _config->hasValue( name.c_str(), "Path", FilePath::none(), &ttfPath ) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup Glyph"
                    , name.c_str()
                );

                return false;
            }

            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            content->setFileGroup( _fileGroup );
            content->setFilePath( ttfPath );

            _font->setContent( content );
        }
        else
        {
            const Char * ttfName = nullptr;
            if( _config->hasValue( name.c_str(), "Name", "", & ttfName) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup Name"
                    , name.c_str()
                );

                return false;
            }

            ConstString groupName;
            Char utf8_ttfPath[MENGINE_MAX_PATH] = {'\0'};
            if( PLATFORM_SERVICE()
                ->getSystemFontPath( &groupName, ttfName, utf8_ttfPath ) == MENGINE_UNKNOWN_SIZE )
            {
                LOGGER_ERROR( "ttf font '%s' don't found '%s' path"
                    , name.c_str()
                    , ttfName
                );

                return false;
            }

            FilePath ttfPath = Helper::stringizeFilePath( utf8_ttfPath );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( groupName );

            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            content->setFileGroup( fileGroup );
            content->setFilePath( ttfPath );

            _font->setContent( content );
        }

        uint32_t height;
        if( _config->hasValue( name.c_str(), "Height", 0u, &height ) == false )
        {
            LOGGER_ERROR( "ttf font '%s' don't setup Height"
                , name.c_str()
            );

            return false;
        }

        _font->setHeight( height );

        FilePath FEPath;
        if( _config->hasValue( name.c_str(), "FEPath", FilePath::none(), &FEPath ) == true )
        {
            FontEffectInterfacePtr fontEffet = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FEFile" ), MENGINE_DOCUMENT_FACTORABLE );

            ContentInterfacePtr content = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "FileContent" ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            content->setFilePath( FEPath );
            content->setFileGroup( _fileGroup );

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
            if( _config->hasValue( name.c_str(), "FESample", 0u, &FESample ) == true )
            {
                fontEffet->setEffectSample( FESample );
            }

            _font->setEffect( fontEffet );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}