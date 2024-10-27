#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ThreadDescription.h"

#include "Config/Char.h"

namespace Mengine
{
    class Win32KernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Win32KernelService" )

    public:
        virtual void setThreadDescription( HANDLE _thread, const ThreadDescription & _description ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_KERNEL_SERVICE()\
    ((Mengine::Win32KernelServiceInterface *)SERVICE_GET(Mengine::Win32KernelServiceInterface))
//////////////////////////////////////////////////////////////////////////