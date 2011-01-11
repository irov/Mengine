/*
 *	ImageDecoderPNG.h
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "libPNG/png.h"

#	include "ImageDecoder.h"

namespace Menge
{
	class ImageDecoderPNG
		: public ImageDecoder
	{
	public:
		ImageDecoderPNG( CodecServiceInterface * _service, InputStreamInterface * _stream );
		~ImageDecoderPNG();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
		void _invalidate() override;

	private:
		png_structp m_png_ptr;
		png_uint_32 m_row_bytes;

		bool m_alphaMask;
		bool m_supportA8;

		void cleanup_();
	};
}	// namespace Menge
