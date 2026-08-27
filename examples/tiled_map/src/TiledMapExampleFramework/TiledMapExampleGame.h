#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TiledMapExampleGame
        : public GameBase
    {
        DECLARE_FACTORABLE( TiledMapExampleGame );

    public:
        TiledMapExampleGame();
        ~TiledMapExampleGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TiledMapExampleGame> TiledMapExampleGamePtr;
    //////////////////////////////////////////////////////////////////////////
}
