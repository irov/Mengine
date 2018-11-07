#include "SDLFileGroupPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "SDLFileGroupDirectory.h"
#include "SDLFileGroupDirectoryFactory.h"

#include "Kernel/FactoryDefault.h"

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
    bool SDLFileGroupPlugin::_initialize()
    {
        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" )
                , new SDLFileGroupDirectoryFactory( STRINGIZE_FILEPATH_LOCAL( "" ) )
            );

        WChar currentPathW[MENGINE_MAX_PATH];
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPathW, MENGINE_MAX_PATH );

        String utf8_currentPath;
        Helper::unicodeToUtf8( currentPathW, utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePath( utf8_currentPath );

        FILE_SERVICE()
            ->registerFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir" )
                , new SDLFileGroupDirectoryFactory( relationPath )
            );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalize()
    {
        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "global" ) );

        FILE_SERVICE()
            ->unregisterFileGroupFactory( STRINGIZE_STRING_LOCAL( "dir") );
    }
}
