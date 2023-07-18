#pragma once

#include "Interface/ThreadSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDLThreadSystem
        : public ServiceBase<ThreadSystemInterface>
    {
    public:
        SDLThreadSystem();
        ~SDLThreadSystem() override;

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
        FactoryInterfacePtr m_factoryThreadIdentity;
        FactoryInterfacePtr m_factoryThreadProcessor;
        FactoryInterfacePtr m_factoryThreadMutex;
        FactoryInterfacePtr m_factoryThreadSharedMutex;
    };
}
