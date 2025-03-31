#include "AppleAppTrackingPlugin.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAppTrackingScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAppTracking, Mengine::AppleAppTrackingPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingPlugin::AppleAppTrackingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingPlugin::~AppleAppTrackingPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingPlugin::_initializePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppTrackingScriptEmbedding" ), Helper::makeFactorableUnique<AppleAppTrackingScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAppTrackingScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingPlugin::_finalizePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
