#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class Box2DGame
        : public GameBase
    {
        DECLARE_FACTORABLE( Box2DGame );

    public:
        Box2DGame();
        ~Box2DGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DGame> Box2DGamePtr;
    //////////////////////////////////////////////////////////////////////////
}