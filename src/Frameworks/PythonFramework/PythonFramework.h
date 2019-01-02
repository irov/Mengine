#pragma once

#include "Interface/FrameworkInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class PythonFramework
        : public ServiceBase<FrameworkInterface>
    {
    public:
        PythonFramework();
        ~PythonFramework() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void onFrameworkInitialize() override;
    };
}
