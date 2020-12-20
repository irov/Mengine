#include "PrefetchReceiver.h"

#include "Kernel/ThreadGuardScope.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::PrefetchReceiver()
        : m_prefetchRefcount( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::~PrefetchReceiver()
    {
        MENGINE_ASSERTION_FATAL( m_prefetchRefcount == 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::initialize( const ThreadTaskPrefetchPtr & _prefetcher )
    {
        m_prefetcher = _prefetcher;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::finalize()
    {
        m_prefetcher = nullptr;

        m_prefetchRefcount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::acquire()
    {
        MENGINE_THREAD_GUARD_SCOPE( PrefetchReceiver, this, "PrefetchReceiver::acquire" );

        ++m_prefetchRefcount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchReceiver::release()
    {
        MENGINE_THREAD_GUARD_SCOPE( PrefetchReceiver, this, "PrefetchReceiver::release" );

        MENGINE_ASSERTION_FATAL( m_prefetchRefcount != 0 );

        if( --m_prefetchRefcount == 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}