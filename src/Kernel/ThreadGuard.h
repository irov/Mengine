#pragma once

#include "Config/Atomic.h"
#include "Config/Char.h"

#ifndef MENGINE_THREAD_GUARD
#   ifdef MENGINE_DEBUG
#       define MENGINE_THREAD_GUARD 1
#   else
#       define MENGINE_THREAD_GUARD 0
#   endif
#endif

namespace Mengine
{
    class ThreadGuard
    {
    public:
        ThreadGuard();
        ~ThreadGuard();

    public:
        void reset();
        void check( const Char * _doc ) const;
        bool lock( bool _value ) const;

    protected:
        AtomicUInt32 m_id;
        mutable AtomicBool m_lock;
    };
}
//////////////////////////////////////////////////////////////////////////
#if MENGINE_THREAD_GUARD
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_INIT\
    public:\
    Mengine::ThreadGuard __mengine_thread_guard\
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_CHECK(Self, Doc)\
    Self->__mengine_thread_guard.check( Doc )
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_RESET(Self)\
    Self->__mengine_thread_guard.reset()
    //////////////////////////////////////////////////////////////////////////
#else
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_INIT
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_CHECK(Self, Doc)
    //////////////////////////////////////////////////////////////////////////
#   define MENGINE_THREAD_GUARD_RESET(Self)
    //////////////////////////////////////////////////////////////////////////
#endif