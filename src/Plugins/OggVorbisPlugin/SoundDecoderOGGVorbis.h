#pragma once

#include "Kernel/SoundDecoder.h"

#include "vorbis/vorbisfile.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundDecoderOGGVorbis
        : public SoundDecoder
    {
    public:
        SoundDecoderOGGVorbis();
        ~SoundDecoderOGGVorbis() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool _prepareData() override;
        bool _rewind() override;

    public:
        size_t _decode( void * _buffer, size_t _bufferSize ) override;

    public:
        bool _seek( float _time ) override;
        float _tell() const override;

    protected:
        OggVorbis_File m_oggVorbisFile;

        bool m_oggVorbisFileInitialize;
    };
}
