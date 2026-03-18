#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class GraphicsGame
        : public GameBase
    {
        DECLARE_FACTORABLE( GraphicsGame );

    public:
        GraphicsGame();
        ~GraphicsGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
