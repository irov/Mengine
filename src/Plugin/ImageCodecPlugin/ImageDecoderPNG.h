#pragma once

#include "Kernel/ImageDecoder.h"

#include "png.h"

namespace Mengine
{
	class ImageDecoderPNG
		: public ImageDecoder
	{
	public:
		ImageDecoderPNG();
		~ImageDecoderPNG() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool _prepareData() override;

	public:
		size_t _decode( void * _buffer, size_t _bufferSize ) override;

	public:
		bool _rewind() override;

	private:
		png_structp m_png_ptr;
		png_infop m_info_ptr;
		png_size_t m_row_bytes;

		int m_interlace_number_of_passes;
	};
}	
