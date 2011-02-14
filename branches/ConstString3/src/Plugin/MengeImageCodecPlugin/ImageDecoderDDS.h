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
		ImageDecoderDDS( CodecServiceInterface * _service, InputStreamInterface * _stream );
		~ImageDecoderDDS();

	public:
		bool initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;
	
	private:
		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge