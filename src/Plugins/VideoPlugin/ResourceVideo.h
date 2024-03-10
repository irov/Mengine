#pragma once

#include "VideoInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceCacher.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceVideo
        : public Resource
        , public UnknownVideoResourceInterface
    {
        DECLARE_FACTORABLE( ResourceVideo );
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceVideo();
        ~ResourceVideo() override;

    public:
        void setFrameRate( float _frameRate ) override;
        float getFrameRate() const override;

    public:
        void setDuration( float _duration ) override;
        float getDuration() const override;

        void setResize( float _resize ) override;
        float getResize() const override;

    public:
        void setAlpha( bool _alpha ) override;
        bool isAlpha() const override;

    public:
        void setPremultiply( bool _premultiply ) override;
        bool isPremultiply() const override;

    public:
        void setNoSeek( bool _noSeek ) override;
        bool isNoSeek() const override;

    public:
        VideoDecoderInterfacePtr createVideoDecoder( const DocumentInterfacePtr & _doc );
        void destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder );

    protected:
        void _cache() override;
        void _uncache() override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        float m_frameRate;
        float m_duration;
        float m_resize;

        typedef ResourceCacher<VideoDecoderInterfacePtr> ResourceCacherVideoDecoder;
        ResourceCacherVideoDecoder m_videoDecoderCacher;

        bool m_alpha;
        bool m_premultiply;
        bool m_noSeek;

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
    protected:
        bool m_forcePremultiply;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceVideo> ResourceVideoPtr;
    //////////////////////////////////////////////////////////////////////////
}
