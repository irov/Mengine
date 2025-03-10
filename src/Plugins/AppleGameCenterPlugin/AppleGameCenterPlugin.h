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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
