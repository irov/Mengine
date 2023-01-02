#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleDevToDevPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleAppTracking" )

    public:
        AppleDevToDevPlugin();
        ~AppleDevToDevPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
