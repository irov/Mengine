#pragma once

#include "Interface/ThreadIdentityInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Atomic.h"

#include <pthread.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXThreadIdentity
        : public ThreadIdentityInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXThreadIdentity );

    public:
        POSIXThreadIdentity();
        ~POSIXThreadIdentity() override;

    public:
        bool initialize( EThreadPriority _priority, const ConstString & _name );
        void finalize();

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda ) override;

    public:
        void main();

    public:
        ThreadId getThreadId() const override;

    public:
        void join() override;
        void cancel() override;

    public:
        bool isCurrentThread() const override;

    public:
        EThreadPriority getPriority() const;

    protected:
        EThreadPriority m_priority;
        ConstString m_name;

        ThreadIdentityRunnerInterfacePtr m_runner;

        pthread_t m_threadId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadIdentity, ThreadIdentityInterface> POSIXThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
