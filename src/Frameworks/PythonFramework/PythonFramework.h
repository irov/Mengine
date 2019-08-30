#pragma once

#include "Kernel/FrameworkBase.h"
#include "Kernel/Observable.h"

namespace Mengine
{
    class PythonFramework
        : public FrameworkBase
        , public Observable
    {
    public:
        PythonFramework();
        ~PythonFramework() override;

    public:
        bool _initializeFramework() override;
        void _finalizeFramework() override;

    protected:
        void notifyBootstrapperInitializeGame();
    };
}
