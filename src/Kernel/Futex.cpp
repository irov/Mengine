#include "Futex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Futex::Futex()
        : m_owner( StdThread::thread::id{} )
        , m_lock( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Futex::~Futex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Futex::lock()
    {
        StdThread::thread::id this_id = StdThread::get_id();

        if( m_owner.load( StdAtomic::memory_order_acquire ) == this_id )
        {
            m_lock.fetch_add( 1, StdAtomic::memory_order_relaxed );

            return;
        }

        StdThread::thread::id no_owner = StdThread::thread::id{};

        while( m_owner.compare_exchange_weak( no_owner, this_id, StdAtomic::memory_order_acquire, StdAtomic::memory_order_relaxed ) == false )
        {
            no_owner = StdThread::thread::id{};

            StdThread::yield();
        }

        m_lock.store( 1, StdAtomic::memory_order_release );
    }
    //////////////////////////////////////////////////////////////////////////
    void Futex::unlock()
    {
        StdThread::thread::id this_id = StdThread::get_id();

        if( m_owner.load( StdAtomic::memory_order_acquire ) != this_id )
        {
            return;
        }

        if( m_lock.fetch_sub( 1, StdAtomic::memory_order_acq_rel ) == 1 )
        {
            m_owner.store( StdThread::thread::id{}, StdAtomic::memory_order_release );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}