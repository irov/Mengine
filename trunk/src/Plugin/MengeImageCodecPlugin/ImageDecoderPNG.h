#	pragma once

#	include "Codec/ImageDecoder.h"

#	include "Core/InputStreamBuffer.h"

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

	public:
		bool _prepareData() override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	private:
		png_structp m_png_ptr;
		png_infop m_info_ptr;
		png_uint_32 m_row_bytes;
		
		InputStreamBuffer8196 m_streamBuffer;
		//stdex::memory_cache m_memories;
	};
}	// namespace Menge
