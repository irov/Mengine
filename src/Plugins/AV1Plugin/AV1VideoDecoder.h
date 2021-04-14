#pragma once

#include "Kernel/VideoDecoder.h"

#include "aom/aom_decoder.h"

namespace Mengine
{
    class AV1VideoDecoder
        : public VideoDecoder
    {
    public:
        AV1VideoDecoder();
        ~AV1VideoDecoder() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool _rewind() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( void * const _buffer, size_t _bufferSize ) override;

    public:
        bool _seek( float _timing ) override;
        float _tell() const override;

    public:
        EVideoDecoderReadState readNextFrame( float _request, float * const _pts ) override;

    public:
        void setPitch( size_t _pitch ) override;
        size_t getPitch() const override;

    protected:
        aom_codec_ctx_t codec;
        AvxVideoReader * reader = NULL;
        const AvxInterface * decoder = NULL;
        const AvxVideoInfo * info = NULL;

        uint32_t m_pitch;

        float m_time;
        bool m_readyFrame;
    };
}
