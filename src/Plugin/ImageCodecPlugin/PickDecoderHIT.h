#pragma once

#include "Interface/ArchiveInterface.h"

#include "Kernel/PickDecoder.h"

namespace Mengine
{
    class PickDecoderHIT
        : public PickDecoder
    {
    public:
        PickDecoderHIT();
        ~PickDecoderHIT() override;

	public:
		bool _initialize() override;

    public:
        bool _prepareData() override;

	public:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;

    private:
        void cleanup_();

    protected:
        uint32_t m_mipmapcompresssize;

		ArchivatorInterfacePtr m_archivator;
    };
}	
