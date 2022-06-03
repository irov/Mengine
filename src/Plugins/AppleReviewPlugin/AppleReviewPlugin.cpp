#include "AppleReviewPlugin.h"

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
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( AppleReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleReviewService );
    }
    //////////////////////////////////////////////////////////////////////////
}
