#include "iOSAppTrackingScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"

#import "iOSAppTrackingPlugin.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static iOSAppTrackingAuthorizationState iOSAppTracking_getAuthorizationState()
        {
            iOSAppTrackingAuthorizationState state = [[iOSAppTrackingPlugin sharedInstance] getAuthorizationState];

            return state;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppTracking_requestAuthorization()
        {
            [[iOSAppTrackingPlugin sharedInstance] authorization];
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppTrackingScriptEmbedding::iOSAppTrackingScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppTrackingScriptEmbedding::~iOSAppTrackingScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSAppTrackingScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<EiOSAppTrackingAuthorization>( _kernel, "iOSAppTrackingAuthorization" )
            .def( "EAATA_AUTHORIZED", EAATA_AUTHORIZED )
            .def( "EAATA_DENIED", EAATA_DENIED )
            .def( "EAATA_RESTRICTED", EAATA_RESTRICTED )
            .def( "EAATA_NOT_DETERMINED", EAATA_NOT_DETERMINED )
            ;

        pybind::enum_<iOSAppTrackingAuthorizationState>( _kernel, "iOSAppTrackingAuthorizationState" )
            .def( "iOSAppTrackingAuthorizationStatePending", iOSAppTrackingAuthorizationStatePending )
            .def( "iOSAppTrackingAuthorizationStateCompleted", iOSAppTrackingAuthorizationStateCompleted )
            .def( "iOSAppTrackingAuthorizationStateFailed", iOSAppTrackingAuthorizationStateFailed )
            ;

        pybind::def_function( _kernel, "iOSAppTrackingRequestAuthorization", &Detail::iOSAppTracking_requestAuthorization );
        pybind::def_function( _kernel, "iOSAppTrackingGetAuthorizationState", &Detail::iOSAppTracking_getAuthorizationState );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAppTrackingScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSAppTrackingAuthorizationStatePending", nullptr );
        _kernel->remove_from_module( "iOSAppTrackingAuthorizationStateCompleted", nullptr );
        _kernel->remove_from_module( "iOSAppTrackingAuthorizationStateFailed", nullptr );

        _kernel->remove_from_module( "iOSAppTrackingRequestAuthorization", nullptr );
        _kernel->remove_from_module( "iOSAppTrackingGetAuthorizationState", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
