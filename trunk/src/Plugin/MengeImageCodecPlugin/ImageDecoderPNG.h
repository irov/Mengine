#	pragma once

#	include "Codec/ImageDecoder.h"

#	include "png.h"

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
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	private:
		png_structp m_png_ptr;
		png_uint_32 m_row_bytes;

		bool m_supportA8;

    protected:
		void cleanup_();
	};
}	// namespace Menge
