#pragma once

#include "Config/Atomic.h"

//////////////////////////////////////////////////////////////////////////
#if !defined(MENGINE_THREAD_GUARD)
#   if defined(MENGINE_DEBUG)
#       define MENGINE_THREAD_GUARD 1
#   else
#       define MENGINE_THREAD_GUARD 0
#   endif
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_THREAD_GUARD == 1
#   define MENGINE_THREAD_GUARD_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class ThreadGuard
    {
    public:
        ThreadGuard();
        ~ThreadGuard();

    public:
        bool lock( bool _value ) const;

    protected:
        mutable AtomicBool m_lock;
    };
}
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_THREAD_GUARD_ENABLE)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_INIT(Type)\
    public:\
    Mengine::ThreadGuard __mengine_thread_guard##Type\
    //////////////////////////////////////////////////////////////////////////
#else
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_INIT(Type)
    //////////////////////////////////////////////////////////////////////////
#endif