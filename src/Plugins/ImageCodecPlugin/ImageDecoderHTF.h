#pragma once

#include "Kernel/ImageDecoder.h"

#include "Interface/ArchivatorInterface.h"

#include "Kernel/Magic.h"

namespace Mengine
{
    class ImageDecoderHTF
        : public ImageDecoder
    {
    public:
        ImageDecoderHTF();
        ~ImageDecoderHTF() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( void * const _buffer, size_t _bufferSize ) override;

    private:
        ArchivatorInterfacePtr m_archivator;
    };
}
