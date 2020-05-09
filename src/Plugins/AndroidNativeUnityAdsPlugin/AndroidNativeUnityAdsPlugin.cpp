#include "AndroidNativeUnityAdsPlugin.h"

#include "AndroidNativeUnityAdsModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

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
            , Helper::makeModuleFactory<AndroidNativeUnityAdsModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeUnityAdsPlugin::_finalizePlugin()
    {
    }
}
