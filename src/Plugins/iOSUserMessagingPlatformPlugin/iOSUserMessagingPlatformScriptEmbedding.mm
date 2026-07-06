#include "iOSUserMessagingPlatformScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "iOSUserMessagingPlatformPlugin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void appleUserMessagingPlatform_showConsentFlow()
        {
            [[iOSUserMessagingPlatformPlugin sharedInstance] showConsentFlow];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleUserMessagingPlatform_isConsentFlowUserGeographyGDPR()
        {
            return [[iOSUserMessagingPlatformPlugin sharedInstance] isConsentFlowUserGeographyGDPR];
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
        pybind::def_function( _kernel, "appleUserMessagingPlatformShowConsentFlow", &Detail::appleUserMessagingPlatform_showConsentFlow );
        pybind::def_function( _kernel, "appleUserMessagingPlatformIsConsentFlowUserGeographyGDPR", &Detail::appleUserMessagingPlatform_isConsentFlowUserGeographyGDPR );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSUserMessagingPlatformScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleUserMessagingPlatformShowConsentFlow", nullptr );
        _kernel->remove_from_module( "appleUserMessagingPlatformIsConsentFlowUserGeographyGDPR", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
