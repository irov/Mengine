#pragma once

#include "Interface/ThreadConditionVariableInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Factorable.h"

#include <pthread.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class POSIXThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( POSIXThreadConditionVariable );

    public:
        POSIXThreadConditionVariable();
        ~POSIXThreadConditionVariable() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void wait( const LambdaCondition & _condition ) override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        pthread_cond_t m_conditionVariable;
        pthread_mutex_t m_conditionLock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<POSIXThreadConditionVariable, ThreadConditionVariableInterface> POSIXThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}
