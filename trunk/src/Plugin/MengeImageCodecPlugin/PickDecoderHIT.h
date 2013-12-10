#	pragma once

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
        size_t decode( void * _buffer, size_t _bufferSize ) override;

    private:
        void cleanup_();

    protected:
        size_t m_mipmapcompresssize;
    };
}	// namespace Menge
