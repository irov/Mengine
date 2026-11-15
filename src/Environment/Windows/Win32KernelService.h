#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/Win32KernelServiceInterface.h"

#include "Kernel/Map.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class Win32KernelService
        : public ServiceBase<Win32KernelServiceInterface>
    {
        DECLARE_FACTORABLE( Win32KernelService );

    public:
        Win32KernelService();
        ~Win32KernelService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setThreadDescription( HANDLE _thread, const ThreadDescription & _description ) const override;

    public:
        void activateSemaphore( const ConstString & _name, const ParamVariant & _value ) override;
        void deactivateSemaphore( const ConstString & _name ) override;
        void waitSemaphore( const ConstString & _name, const SemaphoreListenerInterfacePtr & _listener ) override;
        void clearSemaphores() override;

    protected:
        typedef HRESULT( WINAPI * FWINAPI_SetThreadDescription )(HANDLE, PCWSTR);
        FWINAPI_SetThreadDescription m_WINAPI_SetThreadDescription;

    protected:
        typedef Vector<SemaphoreListenerInterfacePtr> VectorSemaphoreListeners;

        struct SemaphoreDesc
        {
            bool activated = false;
            ParamVariant value = nullptr;
            VectorSemaphoreListeners listeners;
        };

        typedef Map<ConstString, SemaphoreDesc> MapSemaphores;
        MapSemaphores m_semaphores;
        ThreadMutexInterfacePtr m_semaphoresMutex;
    };
}
