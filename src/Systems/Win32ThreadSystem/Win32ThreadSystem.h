#pragma once

#include "Interface/ThreadSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/FactoryWithMutex.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class Win32ThreadSystem
        : public ServiceBase<ThreadSystemInterface>
    {
    public:
        Win32ThreadSystem();
        ~Win32ThreadSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ThreadIdentityInterfacePtr createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadProcessorInterfacePtr createThreadProcessor( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadMutexInterfacePtr createMutex( const DocumentInterfacePtr & _doc ) override;
        ThreadSharedMutexInterfacePtr createSharedMutex( const DocumentInterfacePtr & _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        ThreadId getCurrentThreadId() const override;

    protected:
        FactoryInterfacePtr m_factoryWin32ThreadIdentity;
        FactoryInterfacePtr m_factoryWin32ThreadProcessor;
        FactoryInterfacePtr m_factoryWin32ThreadMutex;
        FactoryInterfacePtr m_factoryWin32ThreadSharedMutex;
    };
}
