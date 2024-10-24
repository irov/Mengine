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
        ThreadIdentityRunnerInterfacePtr run( const LambdaThreadRunner & _lambda, const DocumentInterfacePtr & _doc ) override;

    public:
        void main();

    public:
        EThreadPriority getPriority() const override;
        const ConstString & getName() const override;
        
    public:
        ThreadId getThreadId() const override;

    public:
        void join() override;

    public:
        bool isCurrentThread() const override;

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
