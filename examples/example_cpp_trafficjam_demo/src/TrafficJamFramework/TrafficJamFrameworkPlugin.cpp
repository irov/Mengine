#include "TrafficJamFrameworkPlugin.h"

#include "Interface/FrameworkFactoryInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "TrafficJamFramework.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FrameworkFactory.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( TrafficJamFramework, Mengine::TrafficJamFrameworkPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    TrafficJamFrameworkPlugin::TrafficJamFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TrafficJamFrameworkPlugin::~TrafficJamFrameworkPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TrafficJamFrameworkPlugin::_initializePlugin()
    {
        VOCABULARY_SET( FrameworkFactoryInterface, STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TrafficJamFramework" ), Helper::makeFrameworkFactory<TrafficJamFramework>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TrafficJamFrameworkPlugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Framework" ), STRINGIZE_STRING_LOCAL( "TrafficJamFramework" ) );
    }
}
