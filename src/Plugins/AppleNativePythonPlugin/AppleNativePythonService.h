#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "AppleNativePythonInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleNativePythonService
        : public ServiceBase<AppleNativePythonServiceInterface>
    {
    public:
        AppleNativePythonService();
        ~AppleNativePythonService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void waitSemaphore( const ConstString & _name, const AppleSemaphoreListenerInterfacePtr & _listener ) override;
    };
}
