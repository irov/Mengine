#pragma once

#include "Plugin/VideoPlugin/VideoInterface.h"

#include "Kernel/Decoder.h"

namespace Mengine
{
    class VideoDecoder
        : public Decoder<VideoDecoderInterface>
    {
    public:
        VideoDecoder();
        ~VideoDecoder() override;

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        void setCodecDataInfo( const CodecDataInfo * ) override;
        const VideoCodecDataInfo * getCodecDataInfo() const override;

    protected:
        VideoCodecOptions m_options;
        VideoCodecDataInfo m_dataInfo;
    };
}
