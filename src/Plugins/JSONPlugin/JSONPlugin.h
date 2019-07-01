#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class JSONPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "JSONPython" )

    public:
        JSONPlugin();
        ~JSONPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}

