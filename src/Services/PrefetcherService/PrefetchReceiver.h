#pragma once

#include "Kernel/Factorable.h"

#include "ThreadTaskPrefetch.h"

namespace Mengine
{
    class PrefetchReceiver
        : public Factorable
    {
    public:
        PrefetchReceiver();
        ~PrefetchReceiver() override;

    public:
        void initialize( const ThreadTaskPrefetchPtr & _prefetcher );
        void finalize();

    public:
        void acquire();
        bool release();

    public:
        MENGINE_INLINE const ThreadTaskPrefetchPtr & getPrefetcher() const;

    protected:
        uint32_t m_refcount;
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