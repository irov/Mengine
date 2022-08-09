#include "Win32FileGroupPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Win32FileGroupDirectoryFactory.h"

#include "Kernel/FactoryDefault.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"

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
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), Helper::makeFactory<Win32FileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, FilePath::none() ), MENGINE_DOCUMENT_FACTORABLE );

        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        FilePath relationPath = Helper::stringizeFilePathSize( currentPath, (FilePath::size_type)currentPathLen );

        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), Helper::makeFactory<Win32FileGroupDirectoryFactory>( MENGINE_DOCUMENT_FACTORABLE, relationPath ), MENGINE_DOCUMENT_FACTORABLE );

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