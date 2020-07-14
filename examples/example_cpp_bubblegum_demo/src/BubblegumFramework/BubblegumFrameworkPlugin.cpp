#include "BubblegumFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "BubblegumFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( BubblegumFramework, Mengine::BubblegumFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BubblegumFrameworkPlugin::BubblegumFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    BubblegumFrameworkPlugin::~BubblegumFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool BubblegumFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "BubblegumFramework" ), Helper::makeFrameworkFactory<BubblegumFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void BubblegumFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "BubblegumFramework" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
