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
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const SoundCodecDataInfo * getCodecDataInfo() const override;

    protected:
        SoundCodecOptions m_options;
        SoundCodecDataInfo m_dataInfo;
    };
}
