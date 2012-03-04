/*
 *	ImageDecoderPNG.h
 *
 *	Created by _Berserk_ on 18.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "png.h"

#	include "ImageDecoder.h"

namespace Menge
{
	class LogServiceInterface;

	class ImageDecoderPNG
		: public ImageDecoder
	{
	public:
		ImageDecoderPNG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logService );
		~ImageDecoderPNG();

	public:
		bool initialize() override;

	public:
		LogServiceInterface * getLogService();

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
		void _invalidate() override;

	private:
		LogServiceInterface * m_logService;

		png_structp m_png_ptr;
		png_uint_32 m_row_bytes;

		bool m_supportA8;

		void cleanup_();
	};
}	// namespace Menge
