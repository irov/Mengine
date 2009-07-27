/*
 *	ImageDecoderJPEG.h
 *
 *	Created by _Berserk_ on 3.04.2008
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
		ImageDecoderJPEG( DataStreamInterface* _stream, const String& _type );
		~ImageDecoderJPEG();

	public:
		void destructor() override;
		void release() override;

		const CodecDataInfo* getCodecDataInfo() const override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;

	private:
		int getQuality( jpeg_decompress_struct* _jpegObject );

	private:
		bool m_valid;
		ImageCodecDataInfo m_dataInfo;
		unsigned int m_options;

		bool readHeader_();

		jpeg_decompress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
	};
}	// namespace Menge
