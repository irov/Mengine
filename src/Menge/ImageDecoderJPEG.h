/*
 *	ImageDecoderJPEG.h
 *
 *	Created by _Berserk_ on 22.12.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "DecoderDeclare.h"

struct jpeg_decompress_struct;

namespace Menge
{
	struct tagErrorManager;

	class ImageDecoderJPEG
		: public ImageDecoderInterface
	{
		DECODER_DECLARE( ImageDecoderJPEG );
	public:
		ImageDecoderJPEG( DataStreamInterface* _stream, const String& _type );
		~ImageDecoderJPEG();

	public:
		void destructor() override;
		void release() override;
		const String& getType() const override;

		DataStreamInterface* getStream() override;
		const CodecDataInfo* getCodecDataInfo() const override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;
	private:
		DataStreamInterface* m_stream;
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
