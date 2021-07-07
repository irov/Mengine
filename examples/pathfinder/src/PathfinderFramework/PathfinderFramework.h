#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PathfinderFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( PathfinderFramework );

    public:
        PathfinderFramework();
        ~PathfinderFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
