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
		ImageDecoderMNE();
		~ImageDecoderMNE();

	protected:
		void _initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;

	private:
		bool readHeader_();

	private:
		ImageDecoderJPEG* m_jpegDecoder;
		ImageDecoderPNG* m_pngDecoder;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;

		long int m_png_data_seek;
	};
}	// namespace Menge
