#	pragma once

#	include "Codec/ImageEncoder.h"

#	include <png.h>

namespace Menge
{
	class ImageEncoderPNG
		: public ImageEncoder
	{
	public:
		ImageEncoderPNG();
		~ImageEncoderPNG();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		png_structp m_png_ptr;
	};
}	// namespace Menge
