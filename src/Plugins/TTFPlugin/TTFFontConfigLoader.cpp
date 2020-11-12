#include "TTFFontConfigLoader.h"

#include "Interface/PlatformInterface.h"
#include "Interface/FileServiceInterface.h"

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
            font->setFontColor( colorFont );
        }

        float lineOffset;
        if( _config->hasValue( name.c_str(), "LineOffset", &lineOffset ) == true )
        {
            font->setLineOffset( lineOffset );
        }

        float charOffset;
        if( _config->hasValue( name.c_str(), "CharOffset", &charOffset ) == true )
        {
            font->setCharOffset( charOffset );
        }

        bool system = _config->getValue( name.c_str(), "System", false );

        font->setSystem( system );

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

            font->setTTFPath( ttfPath );

            font->setTTFFileGroup( _fileGroup );
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

            font->setTTFPath( ttfPath );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

            font->setTTFFileGroup( fileGroup );
        }

        uint32_t height;
        if( _config->hasValue( name.c_str(), "Height", &height ) == false )
        {
            LOGGER_ERROR( "invalid font '%s' don't setup Height"
                , name.c_str()
            );

            return false;
        }

        font->setHeight( height );

        FilePath ttfFEPath;
        if( _config->hasValue( name.c_str(), "FEPath", &ttfFEPath ) == true )
        {
            font->setTTFFEPath( ttfFEPath );

            ConstString ttfFEName;
            if( _config->hasValue( name.c_str(), "FEName", &ttfFEName ) == false )
            {
                LOGGER_ERROR( "invalid font '%s' don't setup FEName"
                    , name.c_str()
                );

                return false;
            }

            font->setTTFFEName( ttfFEName );

            font->setTTFFEFileGroup( _fileGroup );
        }

        uint32_t FESample;
        if( _config->hasValue( name.c_str(), "FESample", &FESample ) == true )
        {
            font->setFESample( FESample );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}