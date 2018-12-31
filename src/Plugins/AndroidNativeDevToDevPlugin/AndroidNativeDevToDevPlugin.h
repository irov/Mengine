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
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}
