#pragma once

#include "Kernel/ImageDecoder.h"

#include "Interface/ArchivatorInterface.h"

#include "Kernel/Magic.h"

namespace Mengine
{
    class ImageDecoderACF
        : public ImageDecoder
    {
        DECLARE_FACTORABLE( ImageDecoderACF );

    public:
        ImageDecoderACF();
        ~ImageDecoderACF() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        ArchivatorInterfacePtr m_archivator;
    };
}
