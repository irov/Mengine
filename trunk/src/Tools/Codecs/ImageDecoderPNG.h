/*
 *	ImageDecoderPNG.h
 *
 *	Created by _Berserk_ on 3.04.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageDecoder.h"
#	include "libPNG/png.h"

namespace Menge
{
	class ImageDecoderPNG
		: public ImageDecoder
	{
	public:
		ImageDecoderPNG( DataStreamInterface* _stream, const String& _type );
		~ImageDecoderPNG();

	public:
		void destructor() override;
		void release() override;

		const CodecDataInfo* getCodecDataInfo() const override;
		unsigned int decode( unsigned char* _buffer, unsigned int _bufferSize ) override;

		void setOptions( unsigned int _options ) override;

	private:
		bool m_valid;
		ImageCodecDataInfo m_dataInfo;
		png_structp m_png_ptr;
		unsigned int m_rowStride;
		unsigned int m_bufferRowStride;
		unsigned int m_options;
		std::size_t m_rowsRead;

		bool readHeader_();
		void cleanup_();
	};
}	// namespace Menge
