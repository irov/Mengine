#include "Win32FileSystem.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Win32FileGroupDirectoryFactory.h"
#include "Win32FileGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::Win32FileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileSystem::Win32FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileSystem::~Win32FileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Char currentPath[MENGINE_MAX_PATH + 1] = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}