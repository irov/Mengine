#pragma once

#include "Plugins/VideoPlugin/VideoInterface.h"

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
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const VideoCodecDataInfo * getCodecDataInfo() const override;

    protected:
        VideoCodecDataInfo m_dataInfo;
    };
}
