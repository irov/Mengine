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
        POSIXThreadIdentityRunner( const LambdaThreadRunner & _lambda );
        ~POSIXThreadIdentityRunner() override;

    public:
        void run() override;
        void cancel() override;

    public:
        bool isCancel() const override;

    protected:
        LambdaThreadRunner m_lambda;

        AtomicBool m_cancel;
    };
}