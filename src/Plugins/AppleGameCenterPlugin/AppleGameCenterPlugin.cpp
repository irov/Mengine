#include "AppleGameCenterPlugin.h"
#include "AppleGameCenterInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleGameCenterScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleGameCenterService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleGameCenter, Mengine::AppleGameCenterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterPlugin::AppleGameCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterPlugin::~AppleGameCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applegamecenter" ) == true )
        {
            return true;
        }
        
        if( HAS_OPTION( "noapplegamecenter" ) == true )
        {
            return false;
        }

        bool AppleGameCenterPlugin_Available = CONFIG_VALUE( "AppleGameCenterPlugin", "Available", true );
        
        if( AppleGameCenterPlugin_Available == false )
        {
            return false;
        }
  
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleGameCenterService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

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
    void AppleGameCenterPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleGameCenterService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleGameCenterService );
    }
    //////////////////////////////////////////////////////////////////////////
}
