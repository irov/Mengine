#pragma once

#include "Kernel/VideoDecoder.h"

#include "ogg/ogg.h"
#include "theora/theora.h"

namespace Mengine
{
    class TheoraVideoDecoder
        : public VideoDecoder
    {
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
        size_t _decode( void * _buffer, size_t _bufferSize ) override;

    public:
        bool _seek( float _timing ) override;
        float _tell() const override;

    public:
        EVideoDecoderReadState readNextFrame( float _request, float * _pts ) override;

    public:
        void setPitch( size_t _pitch ) override;
        size_t getPitch() const override;

    protected:
        bool seekToFrame( float _timing );

    protected:
        ogg_stream_state m_oggStreamState;
        ogg_sync_state m_oggSyncState;

        theora_comment m_theoraComment;
        theora_info m_theoraInfo;
        mutable theora_state m_theoraState;

        uint32_t m_pitch;

        float m_time;
        bool m_readyFrame;

    private:
        bool readHeader_();
        size_t read_buffer_data_();
        bool decodeBuffer_( const yuv_buffer & _yuvBuffer, uint8_t * _buffer, size_t _pitch );
        int32_t readFrame_();
    };
}
