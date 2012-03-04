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

	class LogServiceInterface;

	class ImageDecoderJPEG
		: public ImageDecoder
	{
	public:
		ImageDecoderJPEG( CodecServiceInterface * _service, InputStreamInterface * _stream, LogServiceInterface * _logService );
		~ImageDecoderJPEG();

	public:
		bool initialize() override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

	public:
		LogServiceInterface * getLogService();

	protected:
		void _invalidate() override;

	private:
		int getQuality( jpeg_decompress_struct* _jpegObject );

	private:
		LogServiceInterface * m_logService;

		jpeg_decompress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge
