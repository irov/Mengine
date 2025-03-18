#include "AppleAppLovinScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#import "Environment/Python/ApplePythonProvider.h"

#include "AppleAppLovinApplicationDelegate.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

@interface PythonAppleAppLovinConsentFlowProvider : ApplePythonProvider<AppleAppLovinConsentFlowProviderInterface>
@end

@implementation PythonAppleAppLovinConsentFlowProvider

- (void)onAppleAppLovinConsentFlowShowSuccess {
    pybind::object py_cb = [self getMethod:@"onAppleAppLovinConsentFlowShowSuccessful"];
        
    if( py_cb.is_callable() == false ) {
        return;
    }
    
    py_cb.call_args( self.m_args );
}

- (void)onAppleAppLovinConsentFlowShowFailed {
    pybind::object py_cb = [self getMethod:@"onAppleAppLovinConsentFlowShowFailed"];
        
    if( py_cb.is_callable() == false ) {
        return;
    }
    
    py_cb.call_args( self.m_args );
}

@end


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_hasSupportedCMP()
        {
            if( [[AppleAppLovinApplicationDelegate sharedInstance] hasSupportedCMP] == NO )
            {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_isConsentFlowUserGeographyGDPR()
        {
            if( [[AppleAppLovinApplicationDelegate sharedInstance] isConsentFlowUserGeographyGDPR] == NO )
            {
                return false;
            }
            
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_loadAndShowCMPFlow( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<AppleAppLovinConsentFlowProviderInterface> callback
                = [[PythonAppleAppLovinConsentFlowProvider alloc] initWithCbs:_cbs args:_args];
            
            [[AppleAppLovinApplicationDelegate sharedInstance] loadAndShowCMPFlow:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_showMediationDebugger()
        {
            [[AppleAppLovinApplicationDelegate sharedInstance] showMediationDebugger];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinScriptEmbedding::AppleAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleAppLovinScriptEmbedding::~AppleAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleAppLovinScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( STRINGIZE_STRING_LOCAL("AppleAppLovin"), true );
        
        pybind::def_function( _kernel, "appleAppLovinHasSupportedCMP", &Detail::appleAppLovin_hasSupportedCMP );
        pybind::def_function( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", &Detail::appleAppLovin_isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::appleAppLovin_loadAndShowCMPFlow );
        pybind::def_function( _kernel, "appleAppLovinShowMediationDebugger", &Detail::appleAppLovin_showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

