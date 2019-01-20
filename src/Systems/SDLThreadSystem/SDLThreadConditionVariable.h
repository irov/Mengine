#pragma once

#include "Interface/ThreadConditionVariableInterface.h"

#include "Kernel/Factorable.h"

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
        bool initialize( const Char * _file, uint32_t _line );

    protected:
        void wait() override;
        void wake() override;

    protected:
        void _destroy() override;

    protected:
        SDL_cond * m_conditionVariable;
        SDL_mutex * m_conditionLock;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLThreadConditionVariable> SDLThreadConditionVariablePtr;
    //////////////////////////////////////////////////////////////////////////
}