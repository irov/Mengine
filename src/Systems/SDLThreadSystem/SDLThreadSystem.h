#pragma once

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "stdex/pool.h"

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
        ThreadIdentityInterfacePtr createThread( const ConstString & _name, int32_t _priority, const DocumentPtr & _doc ) override;
        ThreadMutexInterfacePtr createMutex( const DocumentPtr & _doc ) override;

    public:
        void sleep( uint32_t _ms ) override;

    public:
        ptrdiff_t getCurrentThreadId() const override;

    protected:
        FactoryPtr m_factoryThreadIdentity;
        FactoryPtr m_factoryThreadMutex;
    };
}
