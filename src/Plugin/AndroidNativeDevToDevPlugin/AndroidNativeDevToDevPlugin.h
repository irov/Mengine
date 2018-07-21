#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class AndroidNativeDevToDevPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "AndroidNativeDevToDev" )

    public:
        AndroidNativeDevToDevPlugin();
        ~AndroidNativeDevToDevPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}
