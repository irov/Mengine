#pragma once

#include "VideoInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"
#include "Kernel/ResourceCacher.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceVideo
        : public Resource
        , public UnknownVideoResourceInterface
        , protected BaseContent        
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();
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
        VideoDecoderInterfacePtr createVideoDecoder( const DocumentPtr & _doc );
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

        typedef ResourceCacher<VideoDecoderInterfacePtr> ResourceCacherVideoDecoder;
        ResourceCacherVideoDecoder m_videoDecoderCacher;

        bool m_alpha;
        bool m_premultiply;
        bool m_noSeek;

#ifndef MENGINE_MASTER_RELEASE
    protected:
        bool m_forcePremultiply;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceVideo> ResourceVideoPtr;
    //////////////////////////////////////////////////////////////////////////
}
