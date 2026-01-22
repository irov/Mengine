#pragma once

#include "Kernel/PluginBase.h"

#include "Win32AntifreezeMonitor.h"

namespace Mengine
{    
    class Win32AntifreezeMonitorPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32AntifreezeMonitor" );

    public:
        Win32AntifreezeMonitorPlugin();
        ~Win32AntifreezeMonitorPlugin() override;

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
        Win32AntifreezeMonitorPtr m_antifreezeMonitor;
    };
}