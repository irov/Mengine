#include "AppleStoreReviewScriptEmbedding.h"

#include "AppleStoreReviewInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleStoreReview_launchTheInAppReview()
        {
            APPLE_STOREREVIEW_SERVICE()
                ->launchTheInAppReview();
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
            ->setAvailablePlugin( "AppleReview", true );

        pybind::def_function( _kernel, "appleStoreReviewLaunchTheInAppReview", &Detail::s_AppleStoreReview_launchTheInAppReview );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleStoreReviewLaunchTheInAppReview", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

