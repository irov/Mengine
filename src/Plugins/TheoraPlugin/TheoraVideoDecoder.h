#pragma once

#include "Kernel/VideoDecoder.h"

#include "ogg/ogg.h"
#include "theora/theora.h"

namespace Mengine
{
    class TheoraVideoDecoder
        : public VideoDecoder
    {
        DECLARE_FACTORABLE( TheoraVideoDecoder );

    public:
        TheoraVideoDecoder();
        ~TheoraVideoDecoder() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool _rewind() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _decoderData ) override;

    public:
        bool _seek( float _timing ) override;
        float _tell() const override;

    public:
        bool isAlphaSplitVideo() const override;

    public:
        void getSurfaceDimension( uint32_t _flags, VideoSurfaceDimension * const _surfaceDimension ) const override;

    public:
        EVideoDecoderReadState readNextFrame( float _request, float * const _pts ) override;

    protected:
        bool seekToFrame( float _timing );

    protected:
        size_t read_buffer_data_();
        bool decodeBuffer_( const yuv_buffer & _yuvBuffer, const VideoDecoderData * _data );

    protected:
        ogg_stream_state m_oggStreamState = {0};
        ogg_sync_state m_oggSyncState = {0};

        theora_comment m_theoraComment = {0};
        theora_info m_theoraInfo = {0};
        mutable theora_state m_theoraState = {0};

        float m_time;
        bool m_readyFrame;
    };
}
