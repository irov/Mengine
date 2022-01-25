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
            bool authorization()
            {
                bool result = APPLE_APPTRACKING_SERVICE()
                    ->authorization();

                return result;
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

        pybind::def_functor( _kernel, "appleGameCenterAuthorization", scriptMethod, &AppleAppTrackingScriptMethod::authorization );

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

