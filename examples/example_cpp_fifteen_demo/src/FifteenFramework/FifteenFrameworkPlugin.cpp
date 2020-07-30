#include "FifteenFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "FifteenFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( FifteenFramework, Mengine::FifteenFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FifteenFrameworkPlugin::FifteenFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FifteenFrameworkPlugin::~FifteenFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FifteenFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "FifteenFramework" ), Helper::makeFrameworkFactory<FifteenFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FifteenFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "FifteenFramework" ) );
    }
}
