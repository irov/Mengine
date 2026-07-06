#include "iOSAppLovinScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonDocument.h"
#import "Environment/Python/ApplePythonProvider.h"

#import "iOSAppLovinPlugin.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

@interface PythoniOSAppLovinConsentFlowProvider : ApplePythonProvider<iOSAppLovinConsentFlowProviderInterface>
@end

@implementation PythoniOSAppLovinConsentFlowProvider

- (void)oniOSAppLovinConsentFlowShowSuccess {
    pybind::object py_cb = [self getMethod:@"oniOSAppLovinConsentFlowShowSuccessful"];

    if( py_cb.is_callable() == false ) {
        return;
    }

    py_cb.call_args( self.m_args );
}

- (void)oniOSAppLovinConsentFlowShowFailed {
    pybind::object py_cb = [self getMethod:@"oniOSAppLovinConsentFlowShowFailed"];

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
            if( [[iOSAppLovinPlugin sharedInstance] hasSupportedCMP] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleAppLovin_isConsentFlowUserGeographyGDPR()
        {
            if( [[iOSAppLovinPlugin sharedInstance] isConsentFlowUserGeographyGDPR] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_loadAndShowCMPFlow( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAppLovinConsentFlowProviderInterface> callback
                = [[PythoniOSAppLovinConsentFlowProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSAppLovinPlugin sharedInstance] loadAndShowCMPFlow:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void appleAppLovin_showMediationDebugger()
        {
            [[iOSAppLovinPlugin sharedInstance] showMediationDebugger];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppLovinScriptEmbedding::iOSAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSAppLovinScriptEmbedding::~iOSAppLovinScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSAppLovinScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "appleAppLovinHasSupportedCMP", &Detail::appleAppLovin_hasSupportedCMP );
        pybind::def_function( _kernel, "appleAppLovinIsConsentFlowUserGeographyGDPR", &Detail::appleAppLovin_isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "appleAppLovinLoadAndShowCMPFlow", &Detail::appleAppLovin_loadAndShowCMPFlow );
        pybind::def_function( _kernel, "appleAppLovinShowMediationDebugger", &Detail::appleAppLovin_showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "appleAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "appleAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "appleAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
