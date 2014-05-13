#	pragma once

#   include "Interface/ArchiveInterface.h"

#	include "Codec/PickDecoder.h"

namespace Menge
{
    class PickDecoderHIT
        : public PickDecoder
    {
    public:
        PickDecoderHIT();
        ~PickDecoderHIT();

	public:
		bool _initialize() override;

    public:
        bool _prepareData() override;

	public:
        size_t decode( void * _buffer, size_t _bufferSize ) override;

    private:
        void cleanup_();

    protected:
        uint32_t m_mipmapcompresssize;

		ArchivatorInterfacePtr m_archivator;
    };
}	// namespace Menge
