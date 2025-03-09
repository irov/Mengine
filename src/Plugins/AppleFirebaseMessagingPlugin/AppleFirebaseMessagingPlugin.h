#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebaseMessagingPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebaseMessaging" )

    public:
        AppleFirebaseMessagingPlugin();
        ~AppleFirebaseMessagingPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
