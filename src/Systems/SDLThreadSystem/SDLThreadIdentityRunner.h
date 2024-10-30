#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Atomic.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    class SDLThreadIdentityRunner
        : public ThreadIdentityRunnerInterface
        , public Factorable
    {
    public:
        SDLThreadIdentityRunner( const LambdaThreadRunner & _lambda, Timestamp _sleep );
        ~SDLThreadIdentityRunner() override;

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