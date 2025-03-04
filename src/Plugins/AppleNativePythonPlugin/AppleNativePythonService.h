#pragma once

#include "Environment/Python/PythonIncluder.h"

#include "AppleNativePythonInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"

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
        
    protected:
        pybind::object addAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args ) override;
        void removeAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb ) override;

    protected:
        void activateSemaphore( const ConstString & _name ) override;
        void waitSemaphore( const ConstString & _name, const AppleSemaphoreListenerInterfacePtr & _listener ) override;
        
    protected:
        struct ApplePythonCallbackDesc
        {
            pybind::object cb;
            pybind::args args;
        };

        typedef Vector<ApplePythonCallbackDesc> VectorApplePythonCallbacks;

        typedef Map<Pair<ConstString, ConstString>, VectorApplePythonCallbacks> MapApplePythonCallbacks;
        MapApplePythonCallbacks m_callbacks;
    };
}
