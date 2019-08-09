#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/ImageEncoder.h"

namespace Mengine
{
    class ImageEncoderACF
        : public ImageEncoder
    {
    public:
        ImageEncoderACF();
        ~ImageEncoderACF() override;

    public:
        bool _initialize() override;

    public:
        size_t encode( const void * _buffer, size_t _size, const CodecDataInfo * _bufferDataInfo ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
