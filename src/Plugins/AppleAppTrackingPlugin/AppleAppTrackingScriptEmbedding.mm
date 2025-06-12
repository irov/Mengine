#include "AppleAppTrackingScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

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
        static void AppleAppTracking_authorization( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;
            
            [[AppleAppTrackingApplicationDelegate sharedInstance] authorization:^(EAppleAppTrackingAuthorization status, NSString * idfa) {
                const Char * idfa_str = [idfa UTF8String];
                
                copy_cb.call_args( status, idfa_str, copy_args );
            }];
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

        pybind::def_function_args( _kernel, "appleAppTrackingAuthorization", &Detail::AppleAppTracking_authorization );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppTrackingAuthorization", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

