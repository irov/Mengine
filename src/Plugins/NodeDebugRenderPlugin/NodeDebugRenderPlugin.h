#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class NodeDebugRenderPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DebugRender" )

    public:
        NodeDebugRenderPlugin();
        ~NodeDebugRenderPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}