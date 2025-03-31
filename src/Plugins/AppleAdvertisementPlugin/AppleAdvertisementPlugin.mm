#include "AppleAdvertisementPlugin.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAdvertisementScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAdvertisement, Mengine::AppleAdvertisementPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementPlugin::AppleAdvertisementPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdvertisementPlugin::~AppleAdvertisementPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementPlugin::_initializePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAdvertisementScriptEmbedding" ), Helper::makeFactorableUnique<AppleAdvertisementScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAdvertisementScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementPlugin::_finalizePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
