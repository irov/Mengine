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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleGeneralDataProtectionRegulation"), true );
        
        AppleGeneralDataProtectionRegulationServiceInterface * service = APPLE_GENERALDATAPROTECTIONREGULATION_SERVICE();

        pybind::def_functor( _kernel, "appleSetGDPRPass", service, &AppleGeneralDataProtectionRegulationServiceInterface::setGDPRPass );
        pybind::def_functor( _kernel, "appleIsGDPRPass", service, &AppleGeneralDataProtectionRegulationServiceInterface::isGDPRPass );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleGeneralDataProtectionRegulation"), false );
            
        _kernel->remove_from_module( "appleSetGDPRPass", nullptr );
        _kernel->remove_from_module( "appleIsGDPRPass", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

