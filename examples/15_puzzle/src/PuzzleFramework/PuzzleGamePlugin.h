#pragma once

#include "PuzzleGame.h"

#include "Kernel/PluginBase.h"

#include "Interface/GameEventReceiverInterface.h"

namespace Mengine
{
    class PuzzleGamePlugin
        : public PluginBase
    {
        PLUGIN_DECLARE( "PuzzleGame" )

    public:
        PuzzleGamePlugin();
        ~PuzzleGamePlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        PuzzleGamePtr m_game;
    };
}