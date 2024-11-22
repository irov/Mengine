#include "PrefetchReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::PrefetchReceiver()
        : m_prefetchRefcount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::~PrefetchReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchReceiver::initialize( const ThreadTaskPrefetchPtr & _prefetcher )
    {
        m_prefetcher = _prefetcher;

        m_prefetchRefcount.incref();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::finalize()
    {
        m_prefetcher = nullptr;

        m_prefetchRefcount.reset();
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::acquire()
    {
        m_prefetchRefcount.incref();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchReceiver::release()
    {
        if( m_prefetchRefcount.decref() != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}