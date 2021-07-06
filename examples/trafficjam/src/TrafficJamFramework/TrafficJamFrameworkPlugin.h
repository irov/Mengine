#pragma once

#include "Kernel/PluginBase.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TrafficJamFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TrafficJamFramework" )

    public:
        TrafficJamFrameworkPlugin();
        ~TrafficJamFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}