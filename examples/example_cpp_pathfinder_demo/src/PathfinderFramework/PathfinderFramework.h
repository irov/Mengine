#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PathfinderFramework
        : public FrameworkBase
    {
    public:
        PathfinderFramework();
        ~PathfinderFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
