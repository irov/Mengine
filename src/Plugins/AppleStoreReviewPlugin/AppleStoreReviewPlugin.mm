#include "AppleStoreReviewPlugin.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
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
    bool AppleStoreReviewPlugin::_initializePlugin()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
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
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
