#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include "SDLThreadIdentity.h"
#   include "SDLThreadMutex.h"

#   include "Factory/FactoryStore.h"

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
        ThreadIdentityPtr createThread(int _priority, const char * _doc) override;
        
    public:
        void sleep( unsigned int _ms ) override;

    public:
        ThreadMutexInterfacePtr createMutex(const char * _doc) override;

    public:
        ptrdiff_t getCurrentThreadId() const override;
    
    protected:
        typedef FactoryPoolStore<SDLThreadIdentity, 16> TPoolThreadIdentity;
        TPoolThreadIdentity m_poolThreadIdentity;

        typedef FactoryPoolStore<SDLThreadMutex, 16> TPoolThreadMutex;
        TPoolThreadMutex m_poolThreadMutex;
    };
}
