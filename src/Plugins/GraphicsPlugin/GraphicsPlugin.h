#pragma once

#include "Kernel/PluginBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class GraphicsPlugin
        : public PluginBase
        , public Observable
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