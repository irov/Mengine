#pragma once

#include "Kernel/GameBase.h"

namespace Mengine
{
    class Arena3DGame
        : public GameBase
    {
        DECLARE_FACTORABLE( Arena3DGame );

    public:
        bool _initializeGame() override;
        void _finalizeGame() override;
    };

    using Arena3DGamePtr = IntrusivePtr<Arena3DGame>;
}
