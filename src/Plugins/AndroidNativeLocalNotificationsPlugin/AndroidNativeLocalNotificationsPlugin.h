#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeLocalNotificationsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeLocalNotifications" )

    public:
        AndroidNativeLocalNotificationsPlugin();
        ~AndroidNativeLocalNotificationsPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
