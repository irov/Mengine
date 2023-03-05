#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class DebugPanelPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DebugPanel" );

    public:
        DebugPanelPlugin();
        ~DebugPanelPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}