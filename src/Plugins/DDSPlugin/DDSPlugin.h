#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class DDSPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "DDS" )

    public:
        DDSPlugin();
        ~DDSPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}