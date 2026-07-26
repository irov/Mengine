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
        static bool iOSAppLovin_hasSupportedCMP()
        {
            if( [[iOSAppLovinPlugin sharedInstance] hasSupportedCMP] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSAppLovin_isConsentFlowUserGeographyGDPR()
        {
            if( [[iOSAppLovinPlugin sharedInstance] isConsentFlowUserGeographyGDPR] == NO )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppLovin_loadAndShowCMPFlow( const pybind::dict & _cbs, const pybind::args & _args )
        {
            id<iOSAppLovinConsentFlowProviderInterface> callback
                = [[PythoniOSAppLovinConsentFlowProvider alloc] initWithCbs:_cbs args:_args];

            [[iOSAppLovinPlugin sharedInstance] loadAndShowCMPFlow:callback];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSAppLovin_showMediationDebugger()
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
        pybind::def_function( _kernel, "iOSAppLovinHasSupportedCMP", &Detail::iOSAppLovin_hasSupportedCMP );
        pybind::def_function( _kernel, "iOSAppLovinIsConsentFlowUserGeographyGDPR", &Detail::iOSAppLovin_isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "iOSAppLovinLoadAndShowCMPFlow", &Detail::iOSAppLovin_loadAndShowCMPFlow );
        pybind::def_function( _kernel, "iOSAppLovinShowMediationDebugger", &Detail::iOSAppLovin_showMediationDebugger );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSAppLovinScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSAppLovinHasSupportedCMP", nullptr );
        _kernel->remove_from_module( "iOSAppLovinIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "iOSAppLovinLoadAndShowCMPFlow", nullptr );
        _kernel->remove_from_module( "iOSAppLovinShowMediationDebugger", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
