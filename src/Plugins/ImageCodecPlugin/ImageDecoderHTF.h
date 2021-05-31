#pragma once

#include "Kernel/ImageDecoder.h"

#include "Interface/ArchivatorInterface.h"

#include "Kernel/Magic.h"

namespace Mengine
{
    class ImageDecoderHTF
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderHTF );

    public:
        ImageDecoderHTF();
        ~ImageDecoderHTF() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _data ) override;

    private:
        ArchivatorInterfacePtr m_archivator;
    };
}
