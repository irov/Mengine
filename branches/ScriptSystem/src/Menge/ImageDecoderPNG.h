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
		ImageDecoderPNG();
		~ImageDecoderPNG();

	protected:
		void _initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;

	private:
		png_structp m_png_ptr;
		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
		std::size_t m_rowsRead;

		bool readHeader_();
		void cleanup_();
	};
}	// namespace Menge
