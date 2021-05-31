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
        uint32_t frameWidth = 0;
        uint32_t frameHeight = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        float fps = 0.f;

        float duration = 0.f;

        EPixelFormat format = PF_UNKNOWN;
        bool clamp = true;

        float getFrameTime() const
        {
            return 1000.f / fps;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    struct VideoCodecOptions
        : public CodecOptions
    {
        float duration = 0.f;
        float fps = 0.f;
        bool alpha = false;
        bool premultiply = false;
    };
    //////////////////////////////////////////////////////////////////////////
    struct VideoDecoderData
        : public DecoderData
    {
        size_t pitch;

        EPixelFormat format;
    };
    //////////////////////////////////////////////////////////////////////////
    class VideoDecoderInterface
        : public DecoderInterface
    {
    public:
        virtual const VideoCodecDataInfo * getCodecDataInfo() const override = 0;

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
