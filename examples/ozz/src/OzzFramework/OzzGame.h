#pragma once

#include "Kernel/GameBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class OzzGame
        : public GameBase
    {
        DECLARE_FACTORABLE( OzzGame );

    public:
        OzzGame();
        ~OzzGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
