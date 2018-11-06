#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DebugRenderPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DebugRender" )

    public:
        DebugRenderPlugin();
        ~DebugRenderPlugin() override;

    protected:
        bool _avaliable() override;

    protected:
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;
    };
}