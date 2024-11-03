#include "SDLFileSystem.h"

#include "Interface/PlatformServiceInterface.h"

#include "SDLFileGroupDirectory.h"
#include "SDLFileGroupDirectoryFactory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::SDLFileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileSystem::SDLFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileSystem::~SDLFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<SDLFileGroupDirectoryFactory, SDLFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Char utf8_currentPath[MENGINE_MAX_PATH + 1] = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( utf8_currentPath );
        
#if defined(MENGINE_PLATFORM_IOS)
        StdString::strcat( utf8_currentPath, "Data/" );
#elif defined(MENGINE_PLATFORM_MACOS)
        StdString::strcat( utf8_currentPath, "Data/" );
#endif
        
        size_t utf8_currentPathLen = StdString::strlen( utf8_currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( utf8_currentPath, (FilePath::size_type)utf8_currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<SDLFileGroupDirectoryFactory, SDLFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
