#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleFirebasePerformanceMonitoringPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleFirebasePerformanceMonitoring" )

    public:
        AppleFirebasePerformanceMonitoringPlugin();
        ~AppleFirebasePerformanceMonitoringPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
