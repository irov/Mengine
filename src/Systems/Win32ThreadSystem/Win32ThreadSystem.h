#pragma once

#include "Interface/ThreadSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Win32ThreadIdentity.h"
#include "Win32ThreadMutex.h"

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
        ThreadIdentityInterfacePtr createThread( const ConstString & _name, EThreadPriority _priority, const DocumentPtr & _doc ) override;
        ThreadMutexInterfacePtr createMutex( const DocumentPtr & _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        uint64_t getCurrentThreadId() const override;

    protected:
        FactoryInterfacePtr m_factoryWin32ThreadIdentity;
        FactoryInterfacePtr m_factoryWin32ThreadMutex;
    };
}
