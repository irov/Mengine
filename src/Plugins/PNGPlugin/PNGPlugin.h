#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PNGPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PNG" );

    public:
        PNGPlugin();
        ~PNGPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}