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
		bool _initialize( bool & _version ) override;

	public:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	private:
		png_structp m_png_ptr;
		png_uint_32 m_row_bytes;

		bool m_supportA8;

    protected:
		void cleanup_();
	};
}	// namespace Menge
