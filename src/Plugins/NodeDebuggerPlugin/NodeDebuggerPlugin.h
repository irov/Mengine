#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class NodeDebuggerPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "NodeDebugger" )

    public:
        NodeDebuggerPlugin();
        ~NodeDebuggerPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}