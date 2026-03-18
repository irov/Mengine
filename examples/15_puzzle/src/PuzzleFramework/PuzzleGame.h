#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PuzzleGame
        : public GameBase
    {
        DECLARE_FACTORABLE( PuzzleGame );

    public:
        PuzzleGame();
        ~PuzzleGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
