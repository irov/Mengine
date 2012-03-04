/*
 *	ImageEncoderPNG.h
 *
 *	Created by _Berserk_ on 22.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageEncoder.h"

#	include "libPNG/png.h"

namespace Menge
{
	class LogServiceInterface;

	class ImageEncoderPNG
		: public ImageEncoder
	{
	public:
		ImageEncoderPNG( CodecServiceInterface * _service, OutputStreamInterface * _stream, LogServiceInterface * _logSystem );
		~ImageEncoderPNG();

	public:
		bool initialize() override;

	public:
		LogServiceInterface * getLogService();

	public:
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		LogServiceInterface * m_logService;

		png_structp m_png_ptr;
	};
}	// namespace Menge
