#pragma once

#include "Interface/ScriptProviderServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class PythonScriptProviderService
        : public ServiceBase<ScriptProviderServiceInterface>
    {
    public:
        PythonScriptProviderService();
        ~PythonScriptProviderService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        pybind::kernel_interface * getKernel() const override;

    protected:
        ThreadId getScriptThreadId() const override;

    protected:
        pybind::kernel_interface * m_kernel;

        ThreadId m_threadId;
    };
}