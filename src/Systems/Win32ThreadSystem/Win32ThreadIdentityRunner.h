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
        Win32ThreadIdentityRunner( const LambdaThreadRunner & _lambda );
        ~Win32ThreadIdentityRunner() override;

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