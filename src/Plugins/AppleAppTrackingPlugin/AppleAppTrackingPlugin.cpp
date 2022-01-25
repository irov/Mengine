#include "AppleAppTrackingPlugin.h"
#include "AppleGameCenterInterface.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "AppleGameCenterScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"

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
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGameCenterScriptEmbedding" ), Helper::makeFactorableUnique<AppleGameCenterScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGameCenterScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingPlugin::_destroyPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
}
