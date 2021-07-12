#pragma once

#include "ThreadTaskPrefetch.h"

#include "Kernel/ReferenceCounter.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrefetchReceiver
        : public Factorable
    {
        DECLARE_FACTORABLE( PrefetchReceiver );

    public:
        PrefetchReceiver();
        ~PrefetchReceiver() override;

    public:
        bool initialize( const ThreadTaskPrefetchPtr & _prefetcher );
        void finalize();

    public:
        void acquire();
        bool release();

    public:
        MENGINE_INLINE const ThreadTaskPrefetchPtr & getPrefetcher() const;

    protected:
        ReferenceCounter m_prefetchRefcount;
        ThreadTaskPrefetchPtr m_prefetcher;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrefetchReceiver> PrefetchReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ThreadTaskPrefetchPtr & PrefetchReceiver::getPrefetcher() const
    {
        return m_prefetcher;
    }
    //////////////////////////////////////////////////////////////////////////
}