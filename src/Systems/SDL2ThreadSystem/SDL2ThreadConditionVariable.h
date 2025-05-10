#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2ThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2ThreadConditionVariable );

    public:
        SDL2ThreadConditionVariable();
        ~SDL2ThreadConditionVariable() override;

    public:
        bool initialize();
        void finalize();

    protected:
        void wait( const LambdaCondition & _condition ) override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2ThreadConditionVariable, ThreadConditionVariableInterface> SDL2ThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}