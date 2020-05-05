#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class TicTacToeFramework
        : public FrameworkBase
    {
    public:
        TicTacToeFramework();
        ~TicTacToeFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
