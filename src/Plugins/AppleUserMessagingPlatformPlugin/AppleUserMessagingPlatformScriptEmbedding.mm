#include "AppleUserMessagingPlatformScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#import "AppleUserMessagingPlatformApplicationDelegate.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void appleUserMessagingPlatform_showConsentFlow()
        {
            [[AppleUserMessagingPlatformApplicationDelegate sharedInstance] showConsentFlow];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool appleUserMessagingPlatform_isConsentFlowUserGeographyGDPR()
        {
            return [[AppleUserMessagingPlatformApplicationDelegate sharedInstance] isConsentFlowUserGeographyGDPR];
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    AppleUserMessagingPlatformScriptEmbedding::AppleUserMessagingPlatformScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleUserMessagingPlatformScriptEmbedding::~AppleUserMessagingPlatformScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleUserMessagingPlatformScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::def_function( _kernel, "appleUserMessagingPlatformShowConsentFlow", &Detail::appleUserMessagingPlatform_showConsentFlow );
        pybind::def_function( _kernel, "appleUserMessagingPlatformIsConsentFlowUserGeographyGDPR", &Detail::appleUserMessagingPlatform_isConsentFlowUserGeographyGDPR );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleUserMessagingPlatformScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "appleUserMessagingPlatformShowConsentFlow", nullptr );
        _kernel->remove_from_module( "appleUserMessagingPlatformIsConsentFlowUserGeographyGDPR", nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}

