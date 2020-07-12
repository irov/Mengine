#include "PrefetchReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::PrefetchReceiver()
        :m_refcount( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PrefetchReceiver::~PrefetchReceiver()
    {
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

        m_refcount = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void PrefetchReceiver::acquire()
    {
        ++m_refcount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PrefetchReceiver::release()
    {
        if( --m_refcount == 0 )
        {
            return false;
        }

        return true;
    }
}