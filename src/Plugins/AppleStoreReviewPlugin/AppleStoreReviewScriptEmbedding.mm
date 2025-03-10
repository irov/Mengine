#include "AppleStoreReviewScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#import "AppleStoreReviewApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void AppleStoreReviewApplicationDelegate_launchTheInAppReview()
        {
            [[AppleStoreReviewApplicationDelegate sharedInstance] launchTheInAppReview];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewScriptEmbedding::AppleStoreReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleStoreReviewScriptEmbedding::~AppleStoreReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleStoreReviewScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleStoreReview"), true );
        
        pybind::def_function( _kernel, "appleStoreReviewLaunchTheInAppReview", &Detail::AppleStoreReviewApplicationDelegate_launchTheInAppReview );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleStoreReviewLaunchTheInAppReview", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

