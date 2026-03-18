#pragma once

#include "GraphicsGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class GraphicsGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "GraphicsGame" )

    public:
        GraphicsGamePlugin();
        ~GraphicsGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        GraphicsGamePtr m_game;
    };
}