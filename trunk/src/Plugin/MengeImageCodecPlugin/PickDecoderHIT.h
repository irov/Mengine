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
        unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

    private:
        void cleanup_();

    protected:
        size_t m_mipmapcompresssize;
    };
}	// namespace Menge
