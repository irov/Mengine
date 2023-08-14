#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Kernel/Factorable.h"

#include "SDL_thread.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDLThreadConditionVariable );

    public:
        SDLThreadConditionVariable();
        ~SDLThreadConditionVariable() override;

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
    typedef IntrusivePtr<SDLThreadConditionVariable, ThreadConditionVariableInterface> SDLThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}
