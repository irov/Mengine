#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/PickEncoder.h"

namespace Mengine
{
    class PickEncoderHIT
        : public PickEncoder
    {
        DECLARE_FACTORABLE( PickEncoderHIT );

    public:
        PickEncoderHIT();
        ~PickEncoderHIT() override;

    public:
        bool _initialize() override;

    public:
        size_t encode( const EncoderData * _encoderData, const CodecDataInfo * _bufferDataInfo ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
