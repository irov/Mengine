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
        , protected BaseContent
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

    public:
        ResourceVideo();
        ~ResourceVideo() override;

    public:
        void setFrameRate( float _frameRate );
        float getFrameRate() const;

    public:
        void setDuration( float _duration );
        float getDuration() const;

    public:
        VideoDecoderInterfacePtr createVideoDecoder( const DocumentPtr & _doc ) const;
        void destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder ) const;

    protected:
        void _cache() override;
        void _uncache() override;

    public:
        void setAlpha( bool _alpha );
        bool isAlpha() const;

        void setNoSeek( bool _noSeek );
        bool isNoSeek() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        float m_frameRate;
        float m_duration;

        typedef ResourceCacher<VideoDecoderInterfacePtr> ResourceCacherVideoDecoder;
        mutable ResourceCacherVideoDecoder m_videoDecoderCacher;

        bool m_alpha;
        bool m_noSeek;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceVideo> ResourceVideoPtr;
    //////////////////////////////////////////////////////////////////////////
}
