#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/ImageEncoder.h"

namespace Mengine
{
    class ImageEncoderACF
        : public ImageEncoder
    {
        DECLARE_FACTORABLE( ImageEncoderACF );

    public:
        ImageEncoderACF();
        ~ImageEncoderACF() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        size_t encode( const EncoderData * _encoderData, const CodecDataInfo * _bufferDataInfo ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
