#pragma once

#include "BubblegumGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class BubblegumGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "BubblegumGame" )

    public:
        BubblegumGamePlugin();
        ~BubblegumGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        BubblegumGamePtr m_game;
    };
}