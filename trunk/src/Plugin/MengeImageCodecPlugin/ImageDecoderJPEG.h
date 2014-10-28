#	pragma once

#	include "Codec/ImageDecoder.h"

extern "C" 
{
#	define XMD_H
#	undef FAR

#	include <setjmp.h>
#	include "jinclude.h"
#	include "jpeglib.h"
#	include "jerror.h"
#	define JPEG_INTERNALS
#	include "jmorecfg.h"
}

namespace Menge
{
	struct DecoderJPEGErrorManager
	{
		struct jpeg_error_mgr pub;
		jmp_buf setjmp_buffer;
	};

	class ImageDecoderJPEG
		: public ImageDecoder
	{
	public:
		ImageDecoderJPEG();
		~ImageDecoderJPEG();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		bool _prepareData() override;

	protected:
		size_t decode( void * _buffer, size_t _bufferSize ) override;

	protected:
		bool _rewind() override;

	private:
		jpeg_decompress_struct m_jpegObject;
		DecoderJPEGErrorManager m_errorMgr;

		bool m_initialize;
	};
}	// namespace Menge
