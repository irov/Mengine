#include "AppleAdvertisementPlugin.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAdvertisementScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleAdvertisementService );
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
    bool AppleAdvertisementPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleadvertisement" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleadvertisement" ) == true )
        {
            return false;
        }
        
        bool AppleAdvertisementPlugin_Available = CONFIG_VALUE_BOOLEAN( "AppleAdvertisementPlugin", "Available", true );

        if( AppleAdvertisementPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdvertisementPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleAdvertisementService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#if defined(MENGINE_USE_SCRIPT_SERVICE)
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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleAdvertisementService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdvertisementPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleAdvertisementService );
    }
    //////////////////////////////////////////////////////////////////////////
}
