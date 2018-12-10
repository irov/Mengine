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
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}