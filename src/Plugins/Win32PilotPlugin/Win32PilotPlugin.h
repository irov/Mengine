#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{    
    class Win32PilotPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Win32Pilot" );

    public:
        Win32PilotPlugin();
        ~Win32PilotPlugin() override;

    protected:
        bool _unimportantPlugin() const override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
