#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebuggerPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "NodeDebugger" )

    public:
        NodeDebuggerPlugin();
        ~NodeDebuggerPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}