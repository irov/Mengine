#include "SDLFileGroupPlugin.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/FileServiceInterface.h"
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

        Char utf8_currentPath[MENGINE_MAX_PATH];
        size_t utf8_currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, utf8_currentPathLen );

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
