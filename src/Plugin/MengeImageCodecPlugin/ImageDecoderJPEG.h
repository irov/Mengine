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
		bool _initialize( bool & _version ) override;

    protected:
		size_t decode( void * _buffer, size_t _bufferSize ) override;
        
	private:
		int getQuality( jpeg_decompress_struct* _jpegObject );

	private:
		jpeg_decompress_struct m_jpegObject;
		DecoderJPEGErrorManager m_errorMgr;
	};
}	// namespace Menge
