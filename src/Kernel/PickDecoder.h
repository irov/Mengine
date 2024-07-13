#pragma once

#include "Interface/PickCodecInterface.h"

#include "Kernel/DecoderBase.h"

namespace Mengine
{
    class PickDecoder
        : public DecoderBase<PickDecoderInterface>
    {
    public:
        PickDecoder();
        ~PickDecoder() override;

    public:
        void setCodecDataInfo( const CodecDataInfo * _dataInfo ) override;
        const PickCodecDataInfo * getCodecDataInfo() const override;

    protected:
        PickCodecDataInfo m_dataInfo;
    };
}
