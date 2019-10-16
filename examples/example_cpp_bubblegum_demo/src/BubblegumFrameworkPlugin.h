#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class BubblegumFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "BubblegumFramework" )

    public:
        BubblegumFrameworkPlugin();
        ~BubblegumFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}