#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Atomic.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    class SDL2ThreadIdentityRunner
        : public ThreadIdentityRunnerInterface
        , public Factorable
    {
    public:
        SDL2ThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep );
        ~SDL2ThreadIdentityRunner() override;

    public:
        bool run() override;
        void cancel() override;

    public:
        void sleep() override;

    public:
        bool isCancel() const override;

    protected:
        LambdaThreadRunner m_lambda;
        Timestamp m_sleep;

        AtomicBool m_cancel;
    };
}