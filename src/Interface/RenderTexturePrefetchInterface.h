#pragma once

#include "Interface/ContentInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/Mixin.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ERenderTexturePrefetchState
    {
        ERTPS_QUEUED,
        ERTPS_DECODING,
        ERTPS_READY_FOR_UPLOAD,
        ERTPS_RESIDENT,
        ERTPS_FAILED,
        ERTPS_CANCELLED
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderTexturePrefetchInterface
        : public Mixin
    {
    public:
        virtual ERenderTexturePrefetchState getState() const = 0;

    public:
        virtual const ContentInterfacePtr & getContent() const = 0;
        virtual RenderTextureInterfacePtr getTexture() const = 0;

    public:
        virtual void cancel() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTexturePrefetchInterface> RenderTexturePrefetchInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const RenderTexturePrefetchInterfacePtr & )> LambdaRenderTexturePrefetch;
    //////////////////////////////////////////////////////////////////////////
}
