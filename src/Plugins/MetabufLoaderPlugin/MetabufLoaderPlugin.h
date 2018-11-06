#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class MetabufLoaderPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "MetabufLoader" );

    public:
        MetabufLoaderPlugin();
        ~MetabufLoaderPlugin() override;

    protected:
        bool _initialize() override;
        void _finalize() override;
    };
}