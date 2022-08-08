#pragma once

#include "Kernel/ThreadGuard.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadGuard;
    //////////////////////////////////////////////////////////////////////////
    class ThreadGuardScope
    {
    public:
        ThreadGuardScope( const ThreadGuard & _guard, const Char * _doc );
        ~ThreadGuardScope();

    private:
        ThreadGuardScope( const ThreadGuardScope & ) = delete;
        void operator = ( const ThreadGuardScope & ) = delete;

    protected:
        const ThreadGuard & m_guard;
        const Char * m_doc;
    };
}
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_THREAD_GUARD_ENABLE
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_SCOPE(Type, Self, Doc)\
    Mengine::ThreadGuardScope __meingine_thread_guard_scope##Type(Self->__mengine_thread_guard##Type, Doc)
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_SCOPE(Type, Self, Doc)
//////////////////////////////////////////////////////////////////////////
#endif

