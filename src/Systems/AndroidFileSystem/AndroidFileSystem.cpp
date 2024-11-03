#include "AndroidFileSystem.h"

#include "AndroidFileGroupDirectory.h"
#include "AndroidAssetGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( FileSystem, Mengine::AndroidFileSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidFileSystem::AndroidFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidFileSystem::~AndroidFileSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileSystem::_initializeService()
    {
        FactoryInterfacePtr factoryGlobalDirGroupDirectory = Helper::makeFactoryDefault<AndroidFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalDirGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        FactoryInterfacePtr factoryAssetGroupDirectory = Helper::makeFactoryDefault<AndroidAssetGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryAssetGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidFileSystem::_finalizeService()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
