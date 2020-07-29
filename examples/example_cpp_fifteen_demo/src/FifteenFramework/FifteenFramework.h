#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FifteenFramework
        : public FrameworkBase
    {
    public:
        FifteenFramework();
        ~FifteenFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
    //////////////////////////////////////////////////////////////////////////
}
