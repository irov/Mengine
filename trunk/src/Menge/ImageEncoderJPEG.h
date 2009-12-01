/*
 *	ImageEncoderJPEG.h
 *
 *	Created by _Berserk_ on 16.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "ImageEncoder.h"

struct jpeg_compress_struct;

namespace Menge
{
	struct tagErrorManager;

	class ImageEncoderJPEG
		: public ImageEncoder
	{
		FACTORABLE_DECLARE( ImageEncoderJPEG );
	public:
		ImageEncoderJPEG();
		~ImageEncoderJPEG();

	public:
		void _initialize() override;

	public:
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		jpeg_compress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;

		//unsigned int m_rowStride;
		//unsigned int m_bufferRowStride;
		bool initializeEncoder_();
	};
}	// namespace Menge
