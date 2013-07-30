#	pragma once

#	include "Interface/ImageCodecInterface.h"

#	include "Codec/ImageEncoder.h"

extern "C" 
{
#	define XMD_H
#	undef FAR

#	include <setjmp.h>

#	include "libJPEG/jinclude.h"
#	include "libJPEG/jpeglib.h"
#	include "libJPEG/jerror.h"
}

namespace Menge
{
	struct EncoderJPEGErrorManager
	{
		jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};

	class ImageEncoderJPEG
		: public ImageEncoder
	{
	public:
		ImageEncoderJPEG();
		~ImageEncoderJPEG();

	public:
		bool _initialize() override;

	public:
		size_t encode( const void * _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		jpeg_compress_struct * m_jpegObject;
		EncoderJPEGErrorManager * m_errorMgr;
	};
}	// namespace Menge
