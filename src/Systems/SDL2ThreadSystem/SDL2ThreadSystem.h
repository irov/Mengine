#pragma once

#include "Interface/ThreadSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDL2ThreadSystem
        : public ServiceBase<ThreadSystemInterface>
    {
    public:
        SDL2ThreadSystem();
        ~SDL2ThreadSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadProcessorInterfacePtr createThreadProcessor( const ThreadDescription & _description, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) override;
        ThreadMutexInterfacePtr createMutex( const DocumentInterfacePtr & _doc ) override;
        ThreadSharedMutexInterfacePtr createSharedMutex( const DocumentInterfacePtr & _doc ) override;
        ThreadConditionVariableInterfacePtr createConditionVariable( const DocumentInterfacePtr & _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        ThreadId getCurrentThreadId() const override;

    protected:
        FactoryInterfacePtr m_factoryThreadIdentity;
        FactoryInterfacePtr m_factoryThreadProcessor;
        FactoryInterfacePtr m_factoryThreadMutex;
        FactoryInterfacePtr m_factoryThreadSharedMutex;
        FactoryInterfacePtr m_factoryThreadConditionVariable;
    };
}