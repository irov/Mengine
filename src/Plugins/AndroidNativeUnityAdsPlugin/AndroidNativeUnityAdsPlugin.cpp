#include "AndroidNativeUnityAdsPlugin.h"

#include "Interface/StringizeServiceInterface.h"

#include "AndroidNativeUnityAdsModule.h"

#include "Kernel/ModuleFactory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeUnityAds, Mengine::AndroidNativeUnityAdsPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsPlugin::AndroidNativeUnityAdsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeUnityAdsPlugin::~AndroidNativeUnityAdsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeUnityAdsPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidUnityAds" )
            , new ModuleFactory<AndroidNativeUnityAdsModule>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsPlugin::_finalizePlugin()
    {
    }    
}
