#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AmplifierPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Amplifier" )

    public:
        AmplifierPlugin();
        ~AmplifierPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}