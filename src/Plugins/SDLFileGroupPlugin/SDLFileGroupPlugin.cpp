#include "SDLFileGroupPlugin.h"

#include "Interface/PlatformInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "SDLFileGroupDirectory.h"
#include "SDLFileGroupDirectoryFactory.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SDLFileGroup, Mengine::SDLFileGroupPlugin)
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupPlugin::SDLFileGroupPlugin()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	SDLFileGroupPlugin::~SDLFileGroupPlugin()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileGroupPlugin::_initializePlugin()
    {
        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( STRINGIZE_FILEPATH_LOCAL( "" ) ) );

        Char utf8_currentPath[MENGINE_MAX_PATH] = {0};
        size_t utf8_currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, utf8_currentPathLen );

        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( relationPath ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
}
