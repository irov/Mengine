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
        ~SDLThreadSystem();

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ThreadIdentityInterfacePtr createThread( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line ) override;
        
    public:
        void sleep( uint32_t _ms ) override;

    public:
        ThreadMutexInterfacePtr createMutex( const Char * _file, uint32_t _line ) override;

    public:
        ptrdiff_t getCurrentThreadId() const override;
    
    protected:
        FactoryPtr m_factoryThreadIdentity;
        FactoryPtr m_factoryThreadMutex;
    };
}
