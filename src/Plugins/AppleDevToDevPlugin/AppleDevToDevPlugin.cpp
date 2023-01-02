#include "AppleDevToDevPlugin.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleDevToDevScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleDevToDevService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAppTracking, Mengine::AppleDevToDevPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleDevToDevPlugin::~AppleDevToDevPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appledevtodev" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noappledevtodev" ) == true )
        {
            return false;
        }
        
        bool AppDevToDevPlugin_Available = CONFIG_VALUE( "AppDevToDevPlugin", "Available", true );
        
        if( AppDevToDevPlugin_Available == false )
        {
            return false;
        }
  
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleDevToDevPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleDevToDevService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleDevToDevScriptEmbedding" ), Helper::makeFactorableUnique<AppleDevToDevScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleDevToDevScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleDevToDevService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleDevToDevPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleDevToDevService );
    }
    //////////////////////////////////////////////////////////////////////////
}
