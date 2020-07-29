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
#if defined(MENGINE_PLATFORM_ANDROID)
SERVICE_EXTERN( AndroidAssetService );
#endif
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( SDLFileGroup, Mengine::SDLFileGroupPlugin )
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
#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_CREATE( AndroidAssetService, MENGINE_DOCUMENT_FACTORABLE );
#endif

        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, STRINGIZE_FILEPATH_LOCAL( "" ) ), MENGINE_DOCUMENT_FACTORABLE );

        Char utf8_currentPath[MENGINE_MAX_PATH] = {0};
        size_t utf8_currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, (FilePath::size_type)utf8_currentPathLen );

        VOCABULARY_SET( Factory, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, relationPath ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalizePlugin()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_FINALIZE( AndroidAssetService );
#endif

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
}
