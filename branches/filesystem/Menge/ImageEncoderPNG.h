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
		FACTORABLE_DECLARE( ImageEncoderPNG );

	public:
		ImageEncoderPNG();
		~ImageEncoderPNG();

	public:
		void _initialize() override;

	public:
		void destructor() override;
		void release() override;

		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		png_structp m_png_ptr;

		bool initializeEncoder_();
	};
}	// namespace Menge
