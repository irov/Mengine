/*
 *	ImageDecoderJPEG.h
 *
 *	Created by _Berserk_ on 4.10.2010
 *	Copyright 2010 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageDecoder.h"

namespace Menge
{
	class ImageDecoderDDS
		: public ImageDecoder
	{
	public:
		ImageDecoderDDS();
		~ImageDecoderDDS();

	public:
		void _initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
		void setOptions( unsigned int _options ) override;

	private:
		bool readHeader_();

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge
