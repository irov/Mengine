#include "iOSUserMessagingPlatformScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "iOSUserMessagingPlatformPlugin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static iOSUserMessagingPlatformConsentState iOSUserMessagingPlatform_getConsentState()
        {
            iOSUserMessagingPlatformConsentState state = [[iOSUserMessagingPlatformPlugin sharedInstance] getConsentState];

            return state;
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSUserMessagingPlatform_retryConsent()
        {
            [[iOSUserMessagingPlatformPlugin sharedInstance] retryConsent];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSUserMessagingPlatform_showConsentFlow()
        {
            [[iOSUserMessagingPlatformPlugin sharedInstance] showConsentFlow];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool iOSUserMessagingPlatform_isConsentFlowUserGeographyGDPR()
        {
            return [[iOSUserMessagingPlatformPlugin sharedInstance] isConsentFlowUserGeographyGDPR];
        }
        //////////////////////////////////////////////////////////////////////////
        static void iOSUserMessagingPlatform_waitForCompletion( const pybind::object & _cb, const pybind::args & _args )
        {
            pybind::object copy_cb = _cb;
            pybind::args copy_args = _args;

            [[iOSUserMessagingPlatformPlugin sharedInstance] waitForConsentCompletion:^{
                copy_cb.call_args( copy_args );
            }];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSUserMessagingPlatformScriptEmbedding::iOSUserMessagingPlatformScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSUserMessagingPlatformScriptEmbedding::~iOSUserMessagingPlatformScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSUserMessagingPlatformScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<iOSUserMessagingPlatformConsentState>( _kernel, "iOSUserMessagingPlatformConsentState" )
            .def( "iOSUserMessagingPlatformConsentStatePending", iOSUserMessagingPlatformConsentStatePending )
            .def( "iOSUserMessagingPlatformConsentStateCompleted", iOSUserMessagingPlatformConsentStateCompleted )
            .def( "iOSUserMessagingPlatformConsentStateFailed", iOSUserMessagingPlatformConsentStateFailed )
            ;

        pybind::def_function( _kernel, "iOSUserMessagingPlatformGetConsentState", &Detail::iOSUserMessagingPlatform_getConsentState );
        pybind::def_function( _kernel, "iOSUserMessagingPlatformRetryConsent", &Detail::iOSUserMessagingPlatform_retryConsent );
        pybind::def_function( _kernel, "iOSUserMessagingPlatformShowConsentFlow", &Detail::iOSUserMessagingPlatform_showConsentFlow );
        pybind::def_function( _kernel, "iOSUserMessagingPlatformIsConsentFlowUserGeographyGDPR", &Detail::iOSUserMessagingPlatform_isConsentFlowUserGeographyGDPR );
        pybind::def_function_args( _kernel, "iOSUserMessagingPlatformWaitForCompletion", &Detail::iOSUserMessagingPlatform_waitForCompletion );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSUserMessagingPlatformScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "iOSUserMessagingPlatformConsentStatePending", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformConsentStateCompleted", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformConsentStateFailed", nullptr );

        _kernel->remove_from_module( "iOSUserMessagingPlatformGetConsentState", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformRetryConsent", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformShowConsentFlow", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformIsConsentFlowUserGeographyGDPR", nullptr );
        _kernel->remove_from_module( "iOSUserMessagingPlatformWaitForCompletion", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
