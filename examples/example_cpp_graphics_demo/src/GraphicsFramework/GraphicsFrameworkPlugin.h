#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GraphicsFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GraphicsFramework" )

    public:
        GraphicsFrameworkPlugin();
        ~GraphicsFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}