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
        bool initialize( const ThreadDescription & _description, EThreadPriority _priority );
        void finalize();

    public:
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc ) override;

    public:
        const ThreadIdentityRunnerInterfacePtr & getRunner() const override;

    public:
        void main( ThreadId _threadId );

    public:
        EThreadPriority getPriority() const override;
        const ThreadDescription & getDescription() const override;
        
    public:
        ThreadId getThreadId() const override;

    public:
        void join( bool _cancel ) override;

    public:
        bool isCurrentThread() const override;

    protected:
        ThreadDescription m_description;
        EThreadPriority m_priority;

        ThreadIdentityRunnerInterfacePtr m_runner;

        pthread_t m_threadId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadIdentity, ThreadIdentityInterface> POSIXThreadIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}
