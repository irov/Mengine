#pragma once

#include "Kernel/SoundDecoder.h"

#include "dr_mp3.h"

namespace Mengine
{
    class SoundDecoderMP3
        : public SoundDecoder
    {
        DECLARE_FACTORABLE( SoundDecoderMP3 );

    public:
        SoundDecoderMP3();
        ~SoundDecoderMP3() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        bool _prepareData() override;
        bool _rewind() override;

    public:
        size_t _decode( const DecoderData * _data ) override;

    public:
        bool _seek( float _time ) override;
        float _tell() const override;

    protected:
        drmp3 m_mp3;

        bool m_mp3Initialize;
    };
}
