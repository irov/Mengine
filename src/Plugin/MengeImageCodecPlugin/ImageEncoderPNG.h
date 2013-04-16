#	pragma once

#	include "Codec/ImageEncoder.h"

#	include "libPNG/png.h"

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
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		png_structp m_png_ptr;
	};
}	// namespace Menge
