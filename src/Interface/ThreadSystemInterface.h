#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadProcessorInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"
#include "Interface/ThreadConditionVariableInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Thread.h"

namespace Mengine
{
    class ThreadSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "ThreadSystem" )

    public:
        virtual ThreadIdentityInterfacePtr createThreadIdentity( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) = 0;
        virtual ThreadProcessorInterfacePtr createThreadProcessor( const ConstString & _name, EThreadPriority _priority, const DocumentInterfacePtr & _doc ) = 0;
        virtual ThreadMutexInterfacePtr createMutex( const DocumentInterfacePtr & _doc ) = 0;
        virtual ThreadSharedMutexInterfacePtr createSharedMutex( const DocumentInterfacePtr & _doc ) = 0;
        virtual ThreadConditionVariableInterfacePtr createConditionVariable( const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual void sleep( uint32_t _ms ) = 0;

    public:
        virtual ThreadId getCurrentThreadId() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define THREAD_SYSTEM()\
    ((Mengine::ThreadSystemInterface *)SERVICE_GET(Mengine::ThreadSystemInterface))
//////////////////////////////////////////////////////////////////////////