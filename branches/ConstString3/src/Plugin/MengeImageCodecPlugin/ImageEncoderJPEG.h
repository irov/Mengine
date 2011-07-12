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
	public:
		ImageEncoderJPEG( CodecServiceInterface * _service, OutputStreamInterface * _stream, LogSystemInterface * _logSystem );
		~ImageEncoderJPEG();

	public:
		bool initialize() override;

	public:
		LogSystemInterface * getLogSystem();

	public:
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		LogSystemInterface * m_logSystem;
		jpeg_compress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;
	};
}	// namespace Menge
