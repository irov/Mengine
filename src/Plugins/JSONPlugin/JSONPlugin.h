#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class JSONPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "JSON" )

    public:
        JSONPlugin();
        ~JSONPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}

