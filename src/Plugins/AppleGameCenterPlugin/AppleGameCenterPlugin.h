#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleGameCenterPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleGameCenter" )

    public:
        AppleGameCenterPlugin();
        ~AppleGameCenterPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
