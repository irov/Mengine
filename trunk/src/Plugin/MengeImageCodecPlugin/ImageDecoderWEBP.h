#	pragma once

#	include "Codec/ImageDecoder.h"

#	include "webp/decode.h"
#	include "webp/mux.h"

namespace Menge
{
	class ImageDecoderWEBP
		: public ImageDecoder
	{
	public:
		ImageDecoderWEBP();
		~ImageDecoderWEBP();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
		void unlockBuffer_();

	protected:
		size_t m_bufferId;
		uint8_t * m_memory;
		size_t m_memorySize;
	};
}

