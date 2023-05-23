#include "AppleAppTrackingScriptEmbedding.h"

#include "AppleAppTrackingInterface.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Factory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleAppTracking_authorization( const pybind::object & _cb, const pybind::args & _args )
        {
            APPLE_APPTRACKING_SERVICE()
                ->authorization( [_cb, _args]( EAppleAppTrackingAuthorization _status, const Char * _idfa )
            {
                _cb.call_args( _status, _idfa, _args );
            } );
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleAppTracking", true );

        pybind::enum_<EAppleAppTrackingAuthorization>( _kernel, "AppleAppTrackingAuthorization" )
            .def( "EAATA_NONE", EAATA_NONE )
            .def( "EAATA_AUTHORIZED", EAATA_AUTHORIZED )
            .def( "EAATA_DENIED", EAATA_DENIED )
            .def( "EAATA_RESTRICTED", EAATA_RESTRICTED )
            .def( "EAATA_NOT_DETERMINED", EAATA_NOT_DETERMINED )
            ;

        pybind::def_function_args( _kernel, "appleAppTrackingAuthorization", &Detail::s_AppleAppTracking_authorization );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppTrackingAuthorization", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

