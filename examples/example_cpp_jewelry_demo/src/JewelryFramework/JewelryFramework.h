#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class JewelryFramework
        : public FrameworkBase
    {
    public:
        JewelryFramework();
        ~JewelryFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
