#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"

namespace Mengine
{
    class ThreadSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ThreadSystem" )

    public:
        virtual ThreadIdentityInterfacePtr createThread( int32_t _priority, const Char * _doc, const Char * _file, uint32_t _line ) = 0;
        virtual ThreadMutexInterfacePtr createMutex( const Char * _file, uint32_t _line ) = 0;
        virtual ThreadConditionVariableInterfacePtr createConditionVariable( const Char * _file, uint32_t _line ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual ptrdiff_t getCurrentThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SYSTEM()\
    ((Mengine::ThreadSystemInterface*)SERVICE_GET(Mengine::ThreadSystemInterface))
//////////////////////////////////////////////////////////////////////////