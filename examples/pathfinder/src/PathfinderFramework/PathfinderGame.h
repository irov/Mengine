#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class PathfinderGame
        : public GameBase
    {
        DECLARE_FACTORABLE( PathfinderGame );

    public:
        PathfinderGame();
        ~PathfinderGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
