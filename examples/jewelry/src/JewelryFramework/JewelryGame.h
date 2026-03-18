#pragma once

#include "Kernel/GameBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class JewelryGame
        : public GameBase
    {
        DECLARE_FACTORABLE( JewelryGame );

    public:
        JewelryGame();
        ~JewelryGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
