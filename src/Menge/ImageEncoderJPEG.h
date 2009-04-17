/*
 *	ImageEncoderJPEG.h
 *
 *	Created by _Berserk_ on 16.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "EncoderDeclare.h"

struct jpeg_compress_struct;

namespace Menge
{
	struct tagErrorManager;

	class ImageEncoderJPEG
		: public ImageEncoderInterface
	{
		ENCODER_DECLARE( ImageEncoderJPEG );
	public:
		ImageEncoderJPEG( OutStreamInterface* _stream, const String& _type );
		~ImageEncoderJPEG();

	public:
		void destructor() override;
		void release() override;
		const String& getType() const override;

		OutStreamInterface* getStream() override;
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

		void setOptions( unsigned int _options ) override;
	private:
		OutStreamInterface* m_stream;
		bool m_valid;
		unsigned int m_options;

		jpeg_compress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;

		//unsigned int m_rowStride;
		//unsigned int m_bufferRowStride;
		bool initializeEncoder_();
	};
}	// namespace Menge
