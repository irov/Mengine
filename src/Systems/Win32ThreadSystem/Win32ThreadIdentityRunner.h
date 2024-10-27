#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class Win32ThreadIdentityRunner
        : public ThreadIdentityRunnerInterface
        , public Factorable
    {
    public:
        Win32ThreadIdentityRunner( const LambdaThreadRunner & _lambda, uint32_t _sleep );
        ~Win32ThreadIdentityRunner() override;

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