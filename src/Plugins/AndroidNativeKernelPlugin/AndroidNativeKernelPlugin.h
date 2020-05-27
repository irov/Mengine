#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeKernelPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeKernel" )

    public:
        AndroidNativeKernelPlugin();
        ~AndroidNativeKernelPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
