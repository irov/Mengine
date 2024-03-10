#include "Win32FileGroupPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"

#include "Win32FileGroupDirectoryFactory.h"
#include "Win32FileGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Win32FileGroup, Mengine::Win32FileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupPlugin::Win32FileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupPlugin::~Win32FileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32FileGroupPlugin::_initializePlugin()
    {
        FactoryInterfacePtr factoryGlobalFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathLen );

        FactoryInterfacePtr factoryDirFileGroupDirectory = Helper::makeFactory<Win32FileGroupDirectoryFactory, Win32FileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE, relationPath );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryDirFileGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}