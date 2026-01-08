#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Environment/SDL3/SDL3Includer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3ThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3ThreadConditionVariable );

    public:
        SDL3ThreadConditionVariable();
        ~SDL3ThreadConditionVariable() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void wait( const LambdaCondition & _condition ) override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        SDL_Condition * m_conditionVariable;
        SDL_Mutex * m_conditionLock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3ThreadConditionVariable, ThreadConditionVariableInterface> SDL3ThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}