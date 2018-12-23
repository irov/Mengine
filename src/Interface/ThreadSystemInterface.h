#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

namespace Mengine
{
    class ThreadSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ThreadSystem" )

    public:
        virtual bool avaliable() const = 0;

    public:
        virtual ThreadIdentityInterfacePtr createThread( int32_t _priority, const Char * _doc, uint32_t _line ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual ThreadMutexInterfacePtr createMutex( const Char * _doc, uint32_t _line ) = 0;

    public:
        virtual ptrdiff_t getCurrentThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SYSTEM()\
    ((Mengine::ThreadSystemInterface*)SERVICE_GET(Mengine::ThreadSystemInterface))
//////////////////////////////////////////////////////////////////////////