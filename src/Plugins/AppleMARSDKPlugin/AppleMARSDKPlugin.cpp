#include "AppleMARSDKPlugin.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleMARSDKScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleMARSDKService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleMARSDK, Mengine::AppleMARSDKPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKPlugin::AppleMARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleMARSDKPlugin::~AppleMARSDKPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noapplemarsdk" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "applemarsdk" ) == false )
        {
            if( CONFIG_VALUE( "AppleMARSDK", "Enable", false ) == false )
            {
                return false;
            }
        }
      
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleMARSDKPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleMARSDKService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleMARSDKScriptEmbedding" ), Helper::makeFactorableUnique<AppleMARSDKScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleMARSDKScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleMARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleMARSDKPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleMARSDKService );
    }
    //////////////////////////////////////////////////////////////////////////
}
