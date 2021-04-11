#pragma once

#include "Interface/DecoderInterface.h"
#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/PixelFormat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EVideoDecoderReadState
    {
        VDRS_FAILURE = 0,
        VDRS_SUCCESS = 1,
        VDRS_END_STREAM = 3
    };
    //////////////////////////////////////////////////////////////////////////
    struct VideoCodecDataInfo
        : public CodecDataInfo
    {
        VideoCodecDataInfo() noexcept
            : frameWidth( 0 )
            , frameHeight( 0 )
            , width( 0 )
            , height( 0 )
            , fps( 0.f )
            , channel( 0 )
            , duration( 0.f )
            , format( PF_UNKNOWN )
            , clamp( true )
        {
        }

        uint32_t frameWidth;
        uint32_t frameHeight;
        uint32_t width;
        uint32_t height;
        float fps;
        uint32_t channel;

        float duration;

        EPixelFormat format;
        bool clamp;

        float getFrameTime() const
        {
            return 1000.f / fps;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    struct VideoCodecOptions
        : public CodecOptions
    {
        VideoCodecOptions()
            : pixelFormat( PF_UNKNOWN )
            , duration( 0.f )
            , fps( 0.f )
            , alpha( false )
            , premultiply( false )
            , mock( false )
            , noSeek( false )
        {
        }

        EPixelFormat pixelFormat;

        float duration;
        float fps;
        bool alpha;
        bool premultiply;

        bool mock;
        bool noSeek;
    };
    //////////////////////////////////////////////////////////////////////////
    class VideoDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const VideoCodecDataInfo * getCodecDataInfo() const override = 0;

    public:
        virtual void setPitch( size_t _pitch ) = 0;
        virtual size_t getPitch() const = 0;

    public:
        virtual EVideoDecoderReadState readNextFrame( float _request, float * const _pts ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VideoDecoderInterface> VideoDecoderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownVideoResourceInterface
        : public UnknownInterface
    {
    public:
        virtual void setFrameRate( float _frameRate ) = 0;
        virtual float getFrameRate() const = 0;

        virtual void setDuration( float _duration ) = 0;
        virtual float getDuration() const = 0;

        virtual void setResize( float _resize ) = 0;
        virtual float getResize() const = 0;

        virtual void setAlpha( bool _alpha ) = 0;
        virtual bool isAlpha() const = 0;

        virtual void setPremultiply( bool _premultiply ) = 0;
        virtual bool isPremultiply() const = 0;

        virtual void setNoSeek( bool _noSeek ) = 0;
        virtual bool isNoSeek() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownVideoSurfaceInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceVideo( const ResourcePtr & _resourceVideo ) = 0;
        virtual const ResourcePtr & getResourceVideo() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
