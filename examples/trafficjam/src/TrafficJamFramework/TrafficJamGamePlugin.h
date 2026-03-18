#pragma once

#include "TrafficJamGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class TrafficJamGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TrafficJamGame" )

    public:
        TrafficJamGamePlugin();
        ~TrafficJamGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        TrafficJamGamePtr m_game;
    };
}