#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaExampleGame
        : public GameBase
    {
        DECLARE_FACTORABLE( VirtualAreaExampleGame );

    public:
        VirtualAreaExampleGame();
        ~VirtualAreaExampleGame() override;

    protected:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VirtualAreaExampleGame> VirtualAreaExampleGamePtr;
    //////////////////////////////////////////////////////////////////////////
}
