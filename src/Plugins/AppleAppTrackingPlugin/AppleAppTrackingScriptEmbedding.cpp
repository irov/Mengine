#include "AppleAppTrackingScriptEmbedding.h"

#include "AppleAppTrackingInterface.h"

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
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class AppleAppTrackingScriptMethod
            : public Factorable
        {
        public:
            AppleAppTrackingScriptMethod()
            {
            }

            ~AppleAppTrackingScriptMethod() override
            {
            }

        public:
            bool initialize()
            {
                return true;
            }

            void finalize()
            {
            }

        public:
            void authorization( const pybind::object & _cb, const pybind::args & _args )
            {
                APPLE_APPTRACKING_SERVICE()
                    ->authorization( [_cb, _args]( EAppleAppTrackingAuthorization _status, const ConstString & _idfa ) {
                        _cb.call_args( _status, _idfa, _args );
                } );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AppleAppTrackingScriptMethod> AppleAppTrackingScriptMethodPtr;
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
        AppleAppTrackingScriptMethodPtr scriptMethod = Helper::makeFactorableUnique<AppleAppTrackingScriptMethod>( MENGINE_DOCUMENT_FACTORABLE );

        if( scriptMethod->initialize() == false )
        {
            return false;
        }

        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleAppTracking", true );
        
        pybind::enum_<EAppleAppTrackingAuthorization>( _kernel, "AppleAppTrackingAuthorization" )
            .def( "EAATA_NONE", EAATA_NONE )
            .def( "EAATA_AUTHORIZED", EAATA_AUTHORIZED )
            .def( "EAATA_DENIED", EAATA_DENIED )
            .def( "EAATA_RESTRICTED", EAATA_RESTRICTED )
            .def( "EAATA_NOT_DETERMINED", EAATA_NOT_DETERMINED )
            ;

        pybind::def_functor_args( _kernel, "appleAppTrackingAuthorization", scriptMethod, &AppleAppTrackingScriptMethod::authorization );

        m_implement = scriptMethod;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppTrackingScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        AppleAppTrackingScriptMethodPtr scriptMethod = m_implement;
        scriptMethod->finalize();

        m_implement = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}

