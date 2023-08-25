#include "AppleGeneralDataProtectionRegulationScriptEmbedding.h"

#include "AppleGeneralDataProtectionRegulationInterface.h"

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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleGeneralDataProtectionRegulation_setGDPRPass( bool _passGDPR )
        {
            APPLE_GENERALDATAPROTECTIONREGULATION_SERVICE()
                ->setGDPRPass( _passGDPR );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_AppleGeneralDataProtectionRegulation_isGDPRPass( bool _passGDPR )
        {
            bool passGDPR = APPLE_GENERALDATAPROTECTIONREGULATION_SERVICE()
                ->isGDPRPass();
            
            return passGDPR;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationScriptEmbedding::AppleGeneralDataProtectionRegulationScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationScriptEmbedding::~AppleGeneralDataProtectionRegulationScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleGeneralDataProtectionRegulation", true );

        pybind::def_function( _kernel, "appleSetGDPRPass", &Detail::s_AppleGeneralDataProtectionRegulation_setGDPRPass );
        pybind::def_function( _kernel, "appleIsGDPRPass", &Detail::s_AppleGeneralDataProtectionRegulation_isGDPRPass );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AppleGeneralDataProtectionRegulation", false );
            
        _kernel->remove_from_module( "appleSetGDPRPass", nullptr );
        _kernel->remove_from_module( "appleIsGDPRPass", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

