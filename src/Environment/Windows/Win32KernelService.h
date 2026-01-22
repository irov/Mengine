#pragma once

#include "Interface/Win32KernelServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32KernelService
        : public ServiceBase<Win32KernelServiceInterface>
    {
        DECLARE_FACTORABLE( Win32KernelService );

    public:
        Win32KernelService();
        ~Win32KernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setThreadDescription( HANDLE _thread, const ThreadDescription & _description ) const override;

    protected:
        typedef HRESULT( WINAPI * FWINAPI_SetThreadDescription )(HANDLE, PCWSTR);
        FWINAPI_SetThreadDescription m_WINAPI_SetThreadDescription;
    };
}