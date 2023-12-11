#pragma once

#include "Interface/ThreadIdentityRunnerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class SDLThreadIdentityRunner
        : public ThreadIdentityRunnerInterface
        , public Factorable
    {
    public:
        SDLThreadIdentityRunner( const LambdaThreadRunner & _lambda );
        ~SDLThreadIdentityRunner() override;

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