#include "AppleAdjustPlugin.h"

#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleAdjustScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleAdjustService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleAdjust, Mengine::AppleAdjustPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustPlugin::AppleAdjustPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAdjustPlugin::~AppleAdjustPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdjustPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "appleadjust" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noappleadjust" ) == true )
        {
            return false;
        }
        
        bool AppleAdjustPlugin_Available = CONFIG_VALUE( "AppleAdjustPlugin", "Available", true );

        if( AppleAdjustPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAdjustPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleAdjustService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAdjustScriptEmbedding" ), Helper::makeFactorableUnique<AppleAdjustScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleAdjustScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdjustPlugin::_finalizePlugin()
    {
#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleAdjustService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAdjustPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleAdjustService );
    }
    //////////////////////////////////////////////////////////////////////////
}
