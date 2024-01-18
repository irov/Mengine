#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class LZ4Plugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "LZ4" );

    public:
        LZ4Plugin();
        ~LZ4Plugin() override;

    protected:
        bool _availablePlugin() const override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
        void _destroyPlugin() override;
    };
}