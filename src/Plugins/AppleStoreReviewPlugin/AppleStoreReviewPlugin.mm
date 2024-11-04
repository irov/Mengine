#include "AppleStoreReviewPlugin.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleStoreReviewScriptEmbedding.h"
#endif

#include "AppleStoreReviewInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleStoreReviewService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleStoreReview, Mengine::AppleStoreReviewPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewPlugin::AppleStoreReviewPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewPlugin::~AppleStoreReviewPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreReviewPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applestorereview" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noapplestorereview" ) == true )
        {
            return false;
        }
        
        bool AppleStoreReviewPlugin_Available = CONFIG_VALUE( "AppleStoreReviewPlugin", "Available", true );
        
        if( AppleStoreReviewPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreReviewPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleStoreReviewService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleStoreReviewScriptEmbedding" ), Helper::makeFactorableUnique<AppleStoreReviewScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleStoreReviewScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
        
        SERVICE_FINALIZE( AppleStoreReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleStoreReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
}
