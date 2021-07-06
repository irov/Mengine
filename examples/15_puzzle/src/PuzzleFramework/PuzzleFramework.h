#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PuzzleFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( PuzzleFramework );

    public:
        PuzzleFramework();
        ~PuzzleFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
