#pragma once

#include "Kernel/ThreadGuard.h"

#include "Kernel/Crash.h"

#include "Config/Char.h"

namespace Mengine
{
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
#if defined(MENGINE_THREAD_GUARD_ENABLE)
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_SCOPE(Type, Self, Doc) Mengine::ThreadGuardScope __meingine_thread_guard_scope##Type( (Self)->__mengine_thread_guard##Type, (Doc) )
#   define MENGINE_THREAD_GUARD_BEGIN(Type, Self, Doc) if( (Self)->__mengine_thread_guard##Type.lock( true ) == true ) Helper::crash( (Doc) )
#   define MENGINE_THREAD_GUARD_END(Type, Self, Doc) if( (Self)->__mengine_thread_guard##Type.lock( false ) == false ) Helper::crash( (Doc) )
//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_SCOPE(Type, Self, Doc)
#   define MENGINE_THREAD_GUARD_BEGIN(Type, Self, Doc)
#   define MENGINE_THREAD_GUARD_END(Type, Self, Doc)
//////////////////////////////////////////////////////////////////////////
#endif

