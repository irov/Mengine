#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "Core/ServiceBase.h"
#   include "Factory/Factory.h"

#   include "stdex/pool.h"

namespace Menge
{
    class SDLThreadSystem
        : public ServiceBase<ThreadSystemInterface>
    {
    public:
        SDLThreadSystem();
        ~SDLThreadSystem();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool avaliable() const override;

    public:
        ThreadIdentityInterfacePtr createThread(int _priority, const char * _file, uint32_t _line ) override;
        
    public:
        void sleep( uint32_t _ms ) override;

    public:
        ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) override;

    public:
        ptrdiff_t getCurrentThreadId() const override;
    
    protected:
        FactoryPtr m_poolThreadIdentity;
        FactoryPtr m_poolThreadMutex;
    };
}
