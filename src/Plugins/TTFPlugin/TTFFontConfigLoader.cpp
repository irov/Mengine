#include "TTFFontConfigLoader.h"

#include "Interface/PlatformInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/FEPlugin/FEInterface.h"

#include "TTFFont.h"

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
    bool TTFFontConfigLoader::load( const TextFontInterfacePtr & _font, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config )
    {
        TTFFontPtr font = _font;

        const ConstString & name = font->getName();

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

        bool system = _config->getValue( name.c_str(), "System", false );

        if( system == false )
        {
            FilePath ttfPath;
            if( _config->hasValue( name.c_str(), "Path", &ttfPath ) == false )
            {
                LOGGER_ERROR( "invalid font '%s' don't setup Glyph"
                    , name.c_str()
                );

                return false;
            }

            ContentInterface * content = _font->getContent();

            content->setFileGroup( _fileGroup );
            content->setFilePath( ttfPath );
        }
        else
        {
            const Char * ttfName = nullptr;
            if( _config->hasValue( name.c_str(), "Name", &ttfName ) == false )
            {
                LOGGER_ERROR( "invalid font '%s' don't setup Name"
                    , name.c_str()
                );

                return false;
            }

            Char utf8_ttfPath[MENGINE_MAX_PATH] = {'\0'};
            if( PLATFORM_SERVICE()
                ->getSystemFontPath( ttfName, utf8_ttfPath ) == MENGINE_UNKNOWN_SIZE )
            {
                LOGGER_ERROR( "invalid font '%s' don't found '%s' path"
                    , name.c_str()
                    , ttfName
                );

                return false;
            }

            FilePath ttfPath = Helper::stringizeFilePath( utf8_ttfPath );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

            ContentInterface * content = _font->getContent();

            content->setFileGroup( fileGroup );
            content->setFilePath( ttfPath );
        }

        uint32_t height;
        if( _config->hasValue( name.c_str(), "Height", &height ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' don't setup Height"
                , name.c_str()
            );

            return false;
        }

        _font->setHeight( height );

        FilePath FEPath;
        if( _config->hasValue( name.c_str(), "FEPath", &FEPath ) == true )
        {
            TextFontEffectInterfacePtr textFontEffet = PROTOTYPE_GENERATE( STRINGIZE_STRING_LOCAL( "FontEffect" ), STRINGIZE_STRING_LOCAL( "FEFile" ), MENGINE_DOCUMENT_FACTORABLE );

            UnknownFEFileInterface * unknownFE = textFontEffet->getDynamicUnknown();

            ContentInterface * content = unknownFE->getContent();

            content->setFilePath( FEPath );
            content->setFileGroup( _fileGroup );

            ConstString FEName;
            if( _config->hasValue( name.c_str(), "FEName", &FEName ) == false )
            {
                LOGGER_ERROR( "invalid font '%s' don't setup FEName"
                    , name.c_str()
                );

                return false;
            }

            unknownFE->setEffectName( FEName );

            uint32_t FESample;
            if( _config->hasValue( name.c_str(), "FESample", &FESample ) == true )
            {
                textFontEffet->setEffectSample( FESample );
            }

            _font->setEffect( textFontEffet );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}