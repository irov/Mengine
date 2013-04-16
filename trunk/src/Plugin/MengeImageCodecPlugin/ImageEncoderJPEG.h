/*
 *	ImageEncoderJPEG.h
 *
 *	Created by _Berserk_ on 16.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

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
	typedef struct tagErrorManager 
	{
		/// "public" fields
		struct jpeg_error_mgr pub;
		/// for return to caller
		jmp_buf setjmp_buffer;
	} ErrorManager;

	class ImageEncoderJPEG
		: public ImageEncoder
	{
	public:
		ImageEncoderJPEG();
		~ImageEncoderJPEG();

	public:
		bool _initialize() override;

	public:
		unsigned int encode( unsigned char* _buffer, const CodecDataInfo* _bufferDataInfo ) override;

	private:
		jpeg_compress_struct* m_jpegObject;
		tagErrorManager* m_errorMgr;
	};
}	// namespace Menge
