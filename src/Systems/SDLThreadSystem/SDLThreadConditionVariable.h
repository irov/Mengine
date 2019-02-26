#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Kernel/Factorable.h"

#include "Config/String.h"
#include "Config/Char.h"

#include "SDL_thread.h"

namespace Mengine
{
    class SDLThreadConditionVariable
        : public ThreadConditionVariableInterface
        , public Factorable
    {
    public:
        SDLThreadConditionVariable();
        ~SDLThreadConditionVariable() override;

    public:
        bool initialize( const Char * _doc );

    protected:
        void wait() override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadConditionVariable> SDLThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}