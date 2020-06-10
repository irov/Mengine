#pragma once

#include "Interface/DecoderInterface.h"

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
            , mock( false )
            , noSeek( false )
        {
        }

        EPixelFormat pixelFormat;

        float duration;
        float fps;
        bool alpha;

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
}
