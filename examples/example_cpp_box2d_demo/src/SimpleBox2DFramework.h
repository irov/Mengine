#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class SimpleBox2DFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        SimpleBox2DFramework();
        ~SimpleBox2DFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
