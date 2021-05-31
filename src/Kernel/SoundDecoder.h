#pragma once

#include "Interface/SoundCodecInterface.h"

#include "Kernel/Decoder.h"

namespace Mengine
{
    class SoundDecoder
        : public Decoder<SoundDecoderInterface>
    {
    public:
        SoundDecoder();
        ~SoundDecoder() override;

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const SoundCodecDataInfo * getCodecDataInfo() const override;

    protected:
        SoundCodecDataInfo m_dataInfo;
    };
}
