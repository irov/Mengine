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
		ImageDecoderPNG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem );
		~ImageDecoderPNG();

	public:
		bool initialize() override;

	public:
		LogSystemInterface * getLogSystem();

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
		void _invalidate() override;

	private:
		LogSystemInterface * m_logSystem;

		png_structp m_png_ptr;
		png_uint_32 m_row_bytes;

		bool m_supportA8;

		void cleanup_();
	};
}	// namespace Menge
