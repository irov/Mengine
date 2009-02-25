/*
 *	ImageDecoderMNE.h
 *
 *	Created by _Berserk_ on 9.2.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "DecoderDeclare.h"

namespace Menge
{
	class ImageDecoderJPEG;
	class ImageDecoderPNG;

	class ImageDecoderMNE
		: public ImageDecoderInterface
	{
		DECODER_DECLARE( ImageDecoderMNE );
	public:
		ImageDecoderMNE( DataStreamInterface* _stream, const String& _type );
		~ImageDecoderMNE();

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

		ImageDecoderJPEG* m_jpegDecoder;
		ImageDecoderPNG* m_pngDecoder;

		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;

		long int m_png_data_seek;
	};
}	// namespace Menge
