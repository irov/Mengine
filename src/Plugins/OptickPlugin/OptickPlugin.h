#pragma once

#include "Kernel/PluginBase.h"

#include "optick.h"

namespace Mengine
{
    class OptickPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Optick" );

    public:
        OptickPlugin();
        ~OptickPlugin() override;

    protected:
        bool _availablePlugin() const override;
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}