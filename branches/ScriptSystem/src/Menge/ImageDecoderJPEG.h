/*
 *	ImageDecoderJPEG.h
 *
 *	Created by _Berserk_ on 22.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageDecoder.h"

struct jpeg_decompress_struct;

namespace Menge
{
	struct tagErrorManager;

	class ImageDecoderJPEG
		: public ImageDecoder
	{
	public:
		ImageDecoderJPEG();
		~ImageDecoderJPEG();

	public:
		void _initialize() override;

	public:
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;

	private:
		int getQuality( jpeg_decompress_struct* _jpegObject );

	private:
		bool readHeader_();

		jpeg_decompress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge
