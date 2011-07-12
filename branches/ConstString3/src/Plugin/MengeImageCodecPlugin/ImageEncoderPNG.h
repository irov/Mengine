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
	class ImageEncoderPNG
		: public ImageEncoder
	{
	public:
		ImageEncoderPNG( CodecServiceInterface * _service, OutputStreamInterface * _stream, LogSystemInterface * _logSystem );
		~ImageEncoderPNG();

	public:
		bool initialize() override;

	public:
		LogSystemInterface * getLogSystem();

	public:
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		LogSystemInterface * m_logSystem;

		png_structp m_png_ptr;
	};
}	// namespace Menge
