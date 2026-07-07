#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class ZstdPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Zstd" );

    public:
        ZstdPlugin();
        ~ZstdPlugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}
