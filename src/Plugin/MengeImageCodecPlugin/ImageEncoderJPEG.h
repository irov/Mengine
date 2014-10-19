#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "Codec/ImageEncoder.h"

extern "C" 
{
#	define XMD_H
#	undef FAR

#	include <setjmp.h>
#	include "jinclude.h"
#	include "jpeglib.h"
#	include "jerror.h"
}

namespace Menge
{
	class ImageEncoderJPEG
		: public ImageEncoder
	{
	public:
		ImageEncoderJPEG();
		~ImageEncoderJPEG();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, const CodecDataInfo * _bufferDataInfo ) override;
	};
}	// namespace Menge
