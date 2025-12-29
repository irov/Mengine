#include "SpinLock.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static uint32_t get_thread_cookie()
        {
            static AtomicUInt32 s_cookie_counter{0};
            static MENGINE_THREAD_LOCAL uint32_t s_thread_cookie = 0;

            if( s_thread_cookie != 0 )
            {
                return s_thread_cookie;
            }

            uint32_t cookie;

            do
            {
                cookie = s_cookie_counter.fetch_add( 1, StdAtomic::memory_order_relaxed ) + 1;
            } while( cookie == 0 );

            s_thread_cookie = cookie;

            return cookie;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SpinLock::SpinLock()
        : m_owner( 0 )
        , m_lock( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpinLock::~SpinLock()
    {
        MENGINE_ASSERTION( m_owner.load( StdAtomic::memory_order_relaxed ) == 0, "SpinLock not released" );
        MENGINE_ASSERTION( m_lock.load( StdAtomic::memory_order_relaxed ) == 0, "SpinLock recursion not zero" );
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinLock::lock()
    {
        uint32_t me = Detail::get_thread_cookie();

        if( m_owner.load( StdAtomic::memory_order_acquire ) == me )
        {
            m_lock.fetch_add( 1, StdAtomic::memory_order_acq_rel );

            return;
        }

        uint32_t expected = 0;

        while( m_owner.compare_exchange_weak( expected, me, StdAtomic::memory_order_acquire, StdAtomic::memory_order_relaxed ) == false )
        {
            expected = 0;

            StdThread::yield();
        }

        m_lock.store( 1, StdAtomic::memory_order_release );
    }
    //////////////////////////////////////////////////////////////////////////
    void SpinLock::unlock()
    {
        uint32_t me = Detail::get_thread_cookie();

        if( m_owner.load( StdAtomic::memory_order_acquire ) != me )
        {
            MENGINE_ASSERTION( false, "SpinLock::unlock invalid owner thread id" );

            return;
        }

        int32_t lock_count = m_lock.fetch_sub( 1, StdAtomic::memory_order_acq_rel );

        MENGINE_ASSERTION( lock_count > 0, "SpinLock::unlock invalid lock count" );

        if( lock_count == 1 )
        {
            m_owner.store( 0, StdAtomic::memory_order_release );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}