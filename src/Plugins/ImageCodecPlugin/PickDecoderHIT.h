#pragma once

#include "Interface/ArchivatorInterface.h"

#include "Kernel/PickDecoder.h"

namespace Mengine
{
    class PickDecoderHIT
        : public PickDecoder
    {
        DECLARE_FACTORABLE( PickDecoderHIT );

    public:
        PickDecoderHIT();
        ~PickDecoderHIT() override;

    public:
        bool _initialize() override;

    public:
        bool _prepareData() override;

    public:
        size_t _decode( const DecoderData * _data ) override;

    protected:
        uint32_t m_mipmapcompresssize;

        ArchivatorInterfacePtr m_archivator;
    };
}
