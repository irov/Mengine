#pragma once

#include "RenderTexturePrefetchQueueInterface.h"

#include "RenderTexturePrefetch.h"

#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTexturePrefetchQueue
        : public RenderTexturePrefetchQueueInterface
    {
        DECLARE_FACTORABLE( RenderTexturePrefetchQueue );

    public:
        RenderTexturePrefetchQueue();
        ~RenderTexturePrefetchQueue() override;

    public:
        RenderTexturePrefetchInterfacePtr request( const ContentInterfacePtr & _content, uint32_t _codecFlags, const LambdaRenderTexturePrefetch & _callback, const DocumentInterfacePtr & _doc ) override;

        void update() override;
        void upload() override;
        void stop() override;

    protected:
        bool isJobUsed_( const RenderTexturePrefetchJobPtr & _job ) const;
        void removeUnusedRequests_();

    protected:
        typedef Vector<RenderTexturePrefetchJobPtr> VectorJobs;
        VectorJobs m_jobs;

        typedef Vector<RenderTexturePrefetchPtr> VectorRequests;
        VectorRequests m_requests;

        bool m_stopped;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexturePrefetchQueue, RenderTexturePrefetchQueueInterface> RenderTexturePrefetchQueuePtr;
    //////////////////////////////////////////////////////////////////////////
}
