#pragma once

#include "TicTacToeGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class TicTacToeGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "TicTacToeGame" )

    public:
        TicTacToeGamePlugin();
        ~TicTacToeGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        TicTacToeGamePtr m_game;
    };
}