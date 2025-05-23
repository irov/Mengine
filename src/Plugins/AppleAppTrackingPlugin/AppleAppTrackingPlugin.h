#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleAppTrackingPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAppTracking" )

    public:
        AppleAppTrackingPlugin();
        ~AppleAppTrackingPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
