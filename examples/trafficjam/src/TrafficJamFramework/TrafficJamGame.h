#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class TrafficJamGame
        : public GameBase
    {
        DECLARE_FACTORABLE( TrafficJamGame );

    public:
        TrafficJamGame();
        ~TrafficJamGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
