#pragma once

#include "Kernel/PluginBase.h"

namespace Mengine
{
    class GraphicsPlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "Graphics" )

    public:
        GraphicsPlugin();
        ~GraphicsPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;
    };
}