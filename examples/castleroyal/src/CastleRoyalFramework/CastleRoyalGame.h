#pragma once

#include "Kernel/GameBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class CastleRoyalGame
        : public GameBase
    {
        DECLARE_FACTORABLE( CastleRoyalGame );

    public:
        CastleRoyalGame();
        ~CastleRoyalGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
}
