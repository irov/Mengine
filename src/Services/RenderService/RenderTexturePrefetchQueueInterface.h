#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderTexturePrefetchInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderTexturePrefetchQueueInterface
        : public ServantInterface
    {
    public:
        virtual RenderTexturePrefetchInterfacePtr request( const ContentInterfacePtr & _content, uint32_t _codecFlags, const LambdaRenderTexturePrefetch & _callback, const DocumentInterfacePtr & _doc ) = 0;

        virtual void update() = 0;
        virtual void upload() = 0;
        virtual void stop() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexturePrefetchQueueInterface> RenderTexturePrefetchQueueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
