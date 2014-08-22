#	pragma once

#	include "Codec/ImageDecoder.h"

#	include "png.h"

//#	include "stdex/memory_cache.h"

namespace Menge
{
	class ImageDecoderPNG
		: public ImageDecoder
	{
	public:
		ImageDecoderPNG();
		~ImageDecoderPNG();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool _prepareData() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
		bool decodeROW_( png_structp png_ptr, void * _buffer );

	private:
		png_uint_32 m_row_bytes;		
	};
}	// namespace Menge
