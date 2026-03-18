#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class BubblegumGame
        : public GameBase
    {
        DECLARE_FACTORABLE( BubblegumGame );

    public:
        BubblegumGame();
        ~BubblegumGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
