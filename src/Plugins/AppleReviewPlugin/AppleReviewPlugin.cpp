#include "AppleReviewPlugin.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleReviewScriptEmbedding.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "AppleReviewInterface.h"
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleReviewService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleReviewPlugin, Mengine::AppleReviewPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleReviewPlugin::AppleReviewPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleReviewPlugin::~AppleReviewPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleReviewPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "noapplereview" ) == true )
        {
            return false;
        }
        
        if( HAS_OPTION( "applereview" ) == false )
        {
            if( CONFIG_VALUE( "AppleReview", "Enable", false ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleReviewPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleReviewService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleReviewScriptEmbedding" ), Helper::makeFactorableUnique<AppleReviewScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleReviewScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
        
        SERVICE_FINALIZE( AppleReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
}
