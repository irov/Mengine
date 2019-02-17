#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Win32ThreadIdentity.h"
#include "Win32ThreadMutex.h"
#include "Win32ThreadConditionVariable.h"

#include "Kernel/Factory.h"
#include "Kernel/FactoryWithMutex.h"

#include "Kernel/ServiceBase.h"

#include "Config/Vector.h"

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
        ThreadIdentityInterfacePtr createThread( const ConstString & _name, int32_t _priority, const Char * _doc ) override;
        ThreadMutexInterfacePtr createMutex( const Char * _doc ) override;
        ThreadConditionVariableInterfacePtr createConditionVariable( const Char * _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        ptrdiff_t getCurrentThreadId() const override;

    protected:
        FactoryPtr m_factoryWin32ThreadIdentity;
        FactoryPtr m_factoryWin32ThreadMutex;
        FactoryWithMutexPtr m_factoryWin32ThreadConditionVariable;

        ThreadMutexInterfacePtr m_mutexConditionVariable;
    };
}
