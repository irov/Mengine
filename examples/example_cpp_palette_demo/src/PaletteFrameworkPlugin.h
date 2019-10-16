#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class PaletteFrameworkPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PaletteFramework" )

    public:
        PaletteFrameworkPlugin();
        ~PaletteFrameworkPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}