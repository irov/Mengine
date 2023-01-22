#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativePythonPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativePython" )

    public:
        AndroidNativePythonPlugin();
        ~AndroidNativePythonPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
