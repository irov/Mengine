#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class POSIXThreadIdentityRunner
        : public ThreadIdentityRunnerInterface
        , public Factorable
    {
    public:
        POSIXThreadIdentityRunner( const LambdaThreadRunner & _lambda, uint32_t _sleep );
        ~POSIXThreadIdentityRunner() override;

    public:
        bool run() override;
        void cancel() override;

    public:
        void sleep() override;

    public:
        bool isCancel() const override;

    protected:
        LambdaThreadRunner m_lambda;
        uint32_t m_sleep;

        AtomicBool m_cancel;
    };
}