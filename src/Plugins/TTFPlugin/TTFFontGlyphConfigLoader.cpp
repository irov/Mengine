#include "TTFFontGlyphConfigLoader.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Plugins/FEPlugin/FEInterface.h"

#include "TTFFontGlyph.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyphConfigLoader::TTFFontGlyphConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFFontGlyphConfigLoader::~TTFFontGlyphConfigLoader()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFontGlyphConfigLoader::load( const FactorablePtr & _factorable, const FileGroupInterfacePtr & _fileGroup, const ConfigInterfacePtr & _config, const DocumentInterfacePtr & _doc )
    {
        TTFFontGlyphPtr glyph = TTFFontGlyphPtr::from( _factorable );

        const ConstString & glyphName = glyph->getName();

        bool system;
        _config->hasValue( glyphName.c_str(), "System", false, &system );

        if( system == false )
        {
            FilePath ttfPath;
            if( _config->hasValue( glyphName.c_str(), "Path", FilePath::none(), &ttfPath ) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup Glyph"
                    , glyphName.c_str()
                );

                return false;
            }

            ContentInterfacePtr glyphContent = Helper::makeFileContent( _fileGroup, ttfPath, _doc );

            DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "not found dataflow 'ttfFont'" );

            glyphContent->setDataflow( dataflow );

            glyph->setGlyphContent( glyphContent );

            FilePath ttfLicense;
            if( _config->hasValue( glyphName.c_str(), "License", FilePath::none(), &ttfLicense ) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup License"
                    , glyphName.c_str()
                );

                return false;
            }

            ContentInterfacePtr licenseContent = Helper::makeFileContent( _fileGroup, ttfLicense, _doc );

            glyph->setLicenseContent( licenseContent );
        }
        else
        {
            const Char * ttfName = nullptr;
            if( _config->hasValue( glyphName.c_str(), "Name", "", & ttfName) == false )
            {
                LOGGER_ERROR( "ttf font '%s' don't setup Name"
                    , glyphName.c_str()
                );

                return false;
            }

            ConstString groupName;
            Path utf8_ttfPath = {'\0'};
            if( PLATFORM_SERVICE()
                ->getSystemFontPath( &groupName, ttfName, utf8_ttfPath ) == MENGINE_UNKNOWN_SIZE )
            {
                LOGGER_ERROR( "ttf font '%s' don't found '%s' path"
                    , glyphName.c_str()
                    , ttfName
                );

                return false;
            }

            FilePath ttfPath = Helper::stringizeFilePath( utf8_ttfPath );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getFileGroup( groupName );

            ContentInterfacePtr glyphContent = Helper::makeFileContent( fileGroup, ttfPath, _doc );

            DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

            MENGINE_ASSERTION_MEMORY_PANIC( dataflow, "not found dataflow 'ttfFont'" );

            glyphContent->setDataflow( dataflow );

            glyph->setGlyphContent( glyphContent );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}