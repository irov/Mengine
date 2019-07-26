#include "AndroidNativeLocalNotificationsPlugin.h"

#include "AndroidNativeLocalNotificationsModule.h"

#include "Interface/StringizeServiceInterface.h"

#include "Kernel/ModuleFactory.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativeLocalNotifications, Mengine::AndroidNativeLocalNotificationsPlugin )
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLocalNotificationsPlugin::AndroidNativeLocalNotificationsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativeLocalNotificationsPlugin::~AndroidNativeLocalNotificationsPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativeLocalNotificationsPlugin::_initializePlugin()
    {
        this->addModuleFactory( STRINGIZE_STRING_LOCAL( "ModuleAndroidLocalNotifications" )
            , Helper::makeModuleFactory<AndroidNativeLocalNotificationsModule>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsPlugin::_finalizePlugin()
    {
    }
}
