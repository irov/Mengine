#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{	
	enum EVideoDecoderReadState
	{
		VDRS_FAILURE = 0,
		VDRS_SUCCESS = 1,
		VDRS_SKIP = 2,
		VDRS_END_STREAM = 3
	};

	struct VideoCodecDataInfo
		: public CodecDataInfo
	{
		size_t frame_width;
		size_t frame_height;
		float duration;
	};
	

	struct VideoCodecOptions
		: public CodecOptions
	{
		PixelFormat pixelFormat;
	};

	class VideoDecoderInterface
		: virtual public DecoderInterface
	{
	public:
		virtual const VideoCodecDataInfo* getCodecDataInfo() const override = 0;

	public:
		virtual int sync( float _timing ) = 0;
		virtual bool seek( float _timing ) = 0;
		virtual bool eof() = 0;
		virtual float getTiming() const = 0;
		virtual EVideoDecoderReadState readNextFrame() = 0;
		
	};
}	// namespace Menge
