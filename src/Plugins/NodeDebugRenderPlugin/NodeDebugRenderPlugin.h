#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeDebugRenderPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DebugRender" )

    public:
        NodeDebugRenderPlugin();
        ~NodeDebugRenderPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}