#include "iOSStoreReviewScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "iOSStoreReviewPlugin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void iOSStoreReviewPlugin_launchTheInAppReview()
        {
            [[iOSStoreReviewPlugin sharedInstance] launchTheInAppReview];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreReviewScriptEmbedding::iOSStoreReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSStoreReviewScriptEmbedding::~iOSStoreReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSStoreReviewScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "appleStoreReviewLaunchTheInAppReview", &Detail::iOSStoreReviewPlugin_launchTheInAppReview );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSStoreReviewScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleStoreReviewLaunchTheInAppReview", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
