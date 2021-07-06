#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class Box2DFramework
        : public FrameworkBase
    {
        DECLARE_FACTORABLE( Box2DFramework );

    public:
        Box2DFramework();
        ~Box2DFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
