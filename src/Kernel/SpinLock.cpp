#include "SpinLock.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SpinLock::SpinLock()
        : m_owner( StdThread::thread::id() )
        , m_lock( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpinLock::~SpinLock()
    {
        MENGINE_ASSERTION( m_owner.load( StdAtomic::memory_order_relaxed ) == StdThread::thread::id(), "SpinLock not released" );
        MENGINE_ASSERTION( m_lock == 0, "SpinLock recursion not zero" );
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinLock::lock()
    {
        StdThread::thread::id threadId = StdThread::get_id();

        if( m_owner.load( StdAtomic::memory_order_acquire ) == threadId )
        {
            ++m_lock;

            return;
        }

        StdThread::thread::id expected;

        while( m_owner.compare_exchange_weak( expected, threadId, StdAtomic::memory_order_acquire, StdAtomic::memory_order_relaxed ) == false )
        {
            expected = StdThread::thread::id();

            StdThread::yield();
        }

        m_lock = 1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpinLock::tryLock()
    {
        StdThread::thread::id threadId = StdThread::get_id();

        if( m_owner.load( StdAtomic::memory_order_acquire ) == threadId )
        {
            ++m_lock;

            return true;
        }

        StdThread::thread::id expected;

        if( m_owner.compare_exchange_strong( expected, threadId, StdAtomic::memory_order_acquire, StdAtomic::memory_order_relaxed ) == false )
        {
            return false;
        }

        m_lock = 1;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinLock::unlock()
    {
        StdThread::thread::id threadId = StdThread::get_id();

        if( m_owner.load( StdAtomic::memory_order_acquire ) != threadId )
        {
            MENGINE_ASSERTION( false, "SpinLock::unlock invalid owner thread id" );

            return;
        }

        MENGINE_ASSERTION( m_lock > 0, "SpinLock::unlock invalid lock count" );

        --m_lock;

        if( m_lock != 0 )
        {
            return;
        }

        m_owner.store( StdThread::thread::id(), StdAtomic::memory_order_release );
    }
    //////////////////////////////////////////////////////////////////////////
}