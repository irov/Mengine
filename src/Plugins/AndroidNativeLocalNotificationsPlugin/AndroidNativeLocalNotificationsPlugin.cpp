#include "AndroidNativeLocalNotificationsPlugin.h"

#include "AndroidNativeLocalNotificationsModule.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"

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
            , Helper::makeModuleFactory<AndroidNativeLocalNotificationsModule>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativeLocalNotificationsPlugin::_finalizePlugin()
    {
    }
}
