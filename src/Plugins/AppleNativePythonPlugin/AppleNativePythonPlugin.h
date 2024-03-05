#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AppleNativePythonPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AppleNativePython" )

    public:
        AppleNativePythonPlugin();
        ~AppleNativePythonPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
