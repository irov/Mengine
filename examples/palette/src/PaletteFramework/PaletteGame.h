#pragma once

#include "Kernel/GameBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PaletteGame
        : public GameBase
    {
        DECLARE_FACTORABLE( PaletteGame );

    public:
        PaletteGame();
        ~PaletteGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
