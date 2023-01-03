#include "AppleHelpshiftPlugin.h"

//#ifdef MENGINE_USE_SCRIPT_SERVICE
//#   include "Interface/ScriptServiceInterface.h"
//
//#   include "AppleHelpshiftScriptEmbedding.h"
//#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleHelpshiftService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleHelpshift, Mengine::AppleHelpshiftPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHelpshiftPlugin::AppleHelpshiftPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHelpshiftPlugin::~AppleHelpshiftPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHelpshiftPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applehelpshift" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "noapplehelpshift" ) == true )
        {
            return false;
        }
        
        bool AppleHelpshiftPlugin_Available = CONFIG_VALUE( "HelpshiftPlugin", "Available", true );

        if( AppleHelpshiftPlugin_Available == false )
        {
            return false;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleHelpshiftPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleHelpshiftService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

//#ifdef MENGINE_USE_SCRIPT_SERVICE
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
//        {
//            SCRIPT_SERVICE()
//                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleHelpshiftScriptEmbedding" ), Helper::makeFactorableUnique<AppleHelpshiftScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//
//        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
//        {
//            SCRIPT_SERVICE()
//                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleHelpshiftScriptEmbedding" ) );
//        }, MENGINE_DOCUMENT_FACTORABLE );
//#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftPlugin::_finalizePlugin()
    {
//#ifdef MENGINE_USE_SCRIPT_SERVICE
//        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
//        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
//#endif

        SERVICE_FINALIZE( AppleHelpshiftService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHelpshiftPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleHelpshiftService );
    }
    //////////////////////////////////////////////////////////////////////////
}
