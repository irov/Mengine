#include "AppleFacebookPlugin.h"

#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleFacebookScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleFacebookService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleFacebook, Mengine::AppleFacebookPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookPlugin::AppleFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFacebookPlugin::~AppleFacebookPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFacebookPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applefacebook" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplefacebook" ) == true )
        {
            return false;
        }

        bool AppleFacebookPlugin_Available = CONFIG_VALUE( "AppleFacebookPlugin", "Available", true );

        if( AppleFacebookPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleFacebookPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleFacebookService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFacebookScriptEmbedding" ), Helper::makeFactorableUnique<AppleFacebookScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleFacebookScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookPlugin::_finalizePlugin()
    {
#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleFacebookService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFacebookPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleFacebookService );
    }
    //////////////////////////////////////////////////////////////////////////
}
