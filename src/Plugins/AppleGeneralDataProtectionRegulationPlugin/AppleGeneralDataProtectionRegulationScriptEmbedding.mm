#include "AppleGeneralDataProtectionRegulationScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#import "AppleGeneralDataProtectionRegulationApplicationDelegate.h"

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
        static void AppleGeneralDataProtectionRegulationApplicationDelegate_setGDPRPass( bool _gdpr )
        {
            [[AppleGeneralDataProtectionRegulationApplicationDelegate sharedInstance] setGDPRPass:_gdpr];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool AppleGeneralDataProtectionRegulationApplicationDelegate_isGDPRPass()
        {
            if ([[AppleGeneralDataProtectionRegulationApplicationDelegate sharedInstance] isGDPRPass] == NO) {
                return false;
            }
            
            return true;
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
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleGeneralDataProtectionRegulation"), true );
        
        pybind::def_function( _kernel, "appleSetGDPRPass", &Detail::AppleGeneralDataProtectionRegulationApplicationDelegate_setGDPRPass );
        pybind::def_function( _kernel, "appleIsGDPRPass", &Detail::AppleGeneralDataProtectionRegulationApplicationDelegate_isGDPRPass );

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

