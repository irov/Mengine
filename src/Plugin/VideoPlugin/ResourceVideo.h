#pragma once

#include "VideoInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceCacher.h"

#include "math/vec2.h"

namespace Mengine
{
    class ResourceVideo
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceVideo();
        ~ResourceVideo() override;

    public:
        bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    public:
        void setFrameRate( float _frameRate );
        float getFrameRate() const;

    public:
        void setDuration( float _duration );
        float getDuration() const;

    public:
        void setFilePath( const FilePath & _path );
        const FilePath & getFilePath() const;

        void setCodecType( const ConstString & _type );
        const ConstString & getCodecType() const;

    public:
        VideoDecoderInterfacePtr createVideoDecoder() const;
        void destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder ) const;

    protected:
        void _cache() override;
        void _uncache() override;

    public:
        void setAlpha( bool _alpha );
        bool isAlpha() const;

    public:
        const mt::vec2f& getFrameSize() const;
        bool isNoSkeep() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_filePath;

        ConstString m_converterType;
        ConstString m_codecType;

        float m_frameRate;
        float m_duration;

        typedef ResourceCacher<VideoDecoderInterfacePtr> TCacherVideoDecoder;
        mutable TCacherVideoDecoder m_videoDecoderCacher;

        bool m_alpha;
        bool m_noSeek;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceVideo> ResourceVideoPtr;
}
