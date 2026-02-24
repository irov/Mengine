#include "AppleStoreReviewScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "AppleStoreReviewApplicationDelegate.h"

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

