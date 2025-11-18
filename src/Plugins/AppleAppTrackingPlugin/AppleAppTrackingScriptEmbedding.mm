#include "AppleAppTrackingScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#import "AppleAppTrackingApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void AppleAppTracking_authorization()
        {
            [[AppleAppTrackingApplicationDelegate sharedInstance] authorization];
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingScriptEmbedding::AppleAppTrackingScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppTrackingScriptEmbedding::~AppleAppTrackingScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppTrackingScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<EAppleAppTrackingAuthorization>( _kernel, "AppleAppTrackingAuthorization" )
            .def( "EAATA_AUTHORIZED", EAATA_AUTHORIZED )
            .def( "EAATA_DENIED", EAATA_DENIED )
            .def( "EAATA_RESTRICTED", EAATA_RESTRICTED )
            .def( "EAATA_NOT_DETERMINED", EAATA_NOT_DETERMINED )
            ;

        pybind::def_function( _kernel, "appleAppTrackingAuthorization", &Detail::AppleAppTracking_authorization );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppTrackingAuthorization", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

