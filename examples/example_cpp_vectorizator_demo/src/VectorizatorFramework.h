#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class VectorizatorFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        VectorizatorFramework();
        ~VectorizatorFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
