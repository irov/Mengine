#include "AndroidFileGroupPlugin.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"

#include "AndroidFileGroupDirectory.h"
#include "AndroidAssetGroupDirectory.h"

#include "Kernel/FactoryHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidFileGroup, Mengine::AndroidFileGroupPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidFileGroupPlugin::AndroidFileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidFileGroupPlugin::~AndroidFileGroupPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidFileGroupPlugin::_initializePlugin()
    {
        FactoryInterfacePtr factoryGlobalDirGroupDirectory = Helper::makeFactoryDefault<AndroidFileGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ), factoryGlobalDirGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        FactoryInterfacePtr factoryAssetGroupDirectory = Helper::makeFactoryDefault<AndroidAssetGroupDirectory>( MENGINE_DOCUMENT_FACTORABLE );
        VOCABULARY_SET( FactoryInterface, STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ), factoryAssetGroupDirectory, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidFileGroupPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "global" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroupFactory" ), STRINGIZE_STRING_LOCAL( "dir" ) );
    }
    ///////////////////////////////////////////////////////////////////////
    void AndroidFileGroupPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
