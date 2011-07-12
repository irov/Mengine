/*
 *	ImageDecoderMNE.h
 *
 *	Created by _Berserk_ on 9.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageDecoder.h"

namespace Menge
{
	class ImageDecoderJPEG;
	class ImageDecoderPNG;

	class ImageDecoderMNE
		: public ImageDecoder
	{
	public:
		ImageDecoderMNE( CodecServiceInterface * _service, InputStreamInterface * _stream, LogSystemInterface * _logSystem );
		~ImageDecoderMNE();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	protected:
		void _invalidate() override;

	private:
		LogSystemInterface * m_logSystem;
		ImageDecoderJPEG* m_jpegDecoder;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;

		long int m_png_data_seek;
	};
}	// namespace Menge
