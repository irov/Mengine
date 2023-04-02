#include "SDLFileGroupPlugin.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "SDLFileGroupDirectory.h"
#include "SDLFileGroupDirectoryFactory.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

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
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, STRINGIZE_FILEPATH_LOCAL( "" ) ), MENGINE_DOCUMENT_FACTORABLE );

        Char utf8_currentPath[MENGINE_MAX_PATH] = {'\0'};
        size_t utf8_currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, (FilePath::size_type)utf8_currentPathLen );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), Helper::makeFactory<SDLFileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, relationPath ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    ///////////////////////////////////////////////////////////////////////
    void SDLFileGroupPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
