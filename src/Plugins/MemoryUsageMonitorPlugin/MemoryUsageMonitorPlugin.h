#pragma once

#include "Kernel/PluginBase.h"

#include "MemoryUsageMonitor.h"

namespace Mengine
{    
    class MemoryUsageMonitorPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MemoryUsageMonitorPlugin" )

    public:
        MemoryUsageMonitorPlugin();
        ~MemoryUsageMonitorPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void notifyPlatformRun_();
        void notifyPlatformUpdate_();
        void notifyPlatformStop_();

    protected:
        MemoryUsageMonitorPtr m_memoryUsageMonitor;
    };
}