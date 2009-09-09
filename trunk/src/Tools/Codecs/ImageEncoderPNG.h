/*
 *	ImageEncoderPNG.h
 *
 *	Created by _Berserk_ on 3.04.2008
 *	Copyright 2008 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "ImageEncoder.h"
#	include "libPNG/png.h"

namespace Menge
{
	class ImageEncoderPNG
		: public ImageEncoder
	{
	public:
		ImageEncoderPNG( OutStreamInterface* _stream, const String& _type );
		~ImageEncoderPNG();

	public:
		void destructor() override;
		void release() override;

		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

		void setOptions( unsigned int _options ) override;

	private:
		bool m_valid;
		png_structp m_png_ptr;
		unsigned int m_options;

		bool initializeEncoder_();
	};
}	// namespace Menge
