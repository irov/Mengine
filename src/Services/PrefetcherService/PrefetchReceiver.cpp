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

        m_prefetchRefcount.increfReferenceCount();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::finalize()
    {
        m_prefetcher = nullptr;

        m_prefetchRefcount.resetReferenceCount();
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::acquire()
    {
        m_prefetchRefcount.increfReferenceCount();
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchReceiver::release()
    {
        uint32_t referenceCount = m_prefetchRefcount.decrefReferenceCount();

        if( referenceCount != 0 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}