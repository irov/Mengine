#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class FifteenGame
        : public GameBase
    {
        DECLARE_FACTORABLE( FifteenGame );

    public:
        FifteenGame();
        ~FifteenGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
