#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleSKAdNetworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleSKAdNetwork" )

    public:
        AppleSKAdNetworkPlugin();
        ~AppleSKAdNetworkPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
