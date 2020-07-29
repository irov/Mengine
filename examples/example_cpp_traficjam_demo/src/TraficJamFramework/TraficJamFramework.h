#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class TraficJamFramework
        : public FrameworkBase
    {
    public:
        TraficJamFramework();
        ~TraficJamFramework() override;

    protected:
        bool _initializeFramework() override;
        void _finalizeFramework() override;
    };
}
