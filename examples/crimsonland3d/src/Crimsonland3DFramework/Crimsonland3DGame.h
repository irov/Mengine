#pragma once

#include "Kernel/GameBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class Crimsonland3DGame
        : public GameBase
    {
        DECLARE_FACTORABLE( Crimsonland3DGame );

    public:
        Crimsonland3DGame();
        ~Crimsonland3DGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Crimsonland3DGame> Crimsonland3DGamePtr;
    //////////////////////////////////////////////////////////////////////////
}
