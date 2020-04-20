#pragma once

#include "Kernel/FrameworkBase.h"

namespace Mengine
{
    class PythonFramework
        : public FrameworkBase
    {
    public:
        PythonFramework();
        ~PythonFramework() override;

    public:
        bool _initializeFramework() override;
        void _finalizeFramework() override;

        bool _runFramework() override;
        void _stopFramework() override;

    protected:
        void notifyBootstrapperInitializeGame();
        void notifyBootstrapperFinalizeGame();
    };
}
