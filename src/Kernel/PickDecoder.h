#pragma once

#include "Interface/PickCodecInterface.h"

#include "Kernel/Decoder.h"

namespace Mengine
{
    class PickDecoder
        : public Decoder<PickDecoderInterface>
    {
    public:
        PickDecoder();
        ~PickDecoder() override;

    public:
        bool setOptions( const CodecOptions * _options ) override;

    protected:
        virtual bool _invalidateOptions();

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const PickCodecDataInfo * getCodecDataInfo() const override;

    protected:
        PickCodecOptions m_options;
        PickCodecDataInfo m_dataInfo;
    };
}
