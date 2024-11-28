#include "AppleStoreReviewScriptEmbedding.h"

#include "AppleStoreReviewInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
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
        
        AppleStoreReviewServiceInterface * service = APPLE_STOREREVIEW_SERVICE();

        pybind::def_functor( _kernel, "appleStoreReviewLaunchTheInAppReview", service, &AppleStoreReviewServiceInterface::launchTheInAppReview );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleStoreReviewScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleStoreReviewLaunchTheInAppReview", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

