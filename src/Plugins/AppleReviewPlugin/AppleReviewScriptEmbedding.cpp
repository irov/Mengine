#include "AppleReviewScriptEmbedding.h"

#include "AppleReviewInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleReview_launchTheInAppReview()
        {
            APPLE_REVIEW_SERVICE()
                ->launchTheInAppReview();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleReviewScriptEmbedding::AppleReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleReviewScriptEmbedding::~AppleReviewScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleReviewScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleReview", true );

        pybind::def_function( _kernel, "appleReviewLaunchTheInAppReview", &Detail::s_AppleReview_launchTheInAppReview );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleReviewScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleReviewLaunchTheInAppReview", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

