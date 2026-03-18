#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class TicTacToeGame
        : public GameBase
    {
        DECLARE_FACTORABLE( TicTacToeGame );

    public:
        TicTacToeGame();
        ~TicTacToeGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
