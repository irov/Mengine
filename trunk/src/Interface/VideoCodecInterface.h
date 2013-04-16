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
		int frameWidth;
		int frameHeight;
		float frameTiming;
		float fps;
		float duration;
	};
	

	struct VideoCodecOptions
		: public CodecOptions
	{
        VideoCodecOptions()
            : pixelFormat(PF_UNKNOWN)
            , noSeek(false)
        {}

		PixelFormat pixelFormat;
        bool noSeek;
	};

	class VideoDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const VideoCodecDataInfo* getCodecDataInfo() const override = 0;

	public:
		//virtual int sync( float _timing ) = 0;
		virtual bool seek( float _timing ) = 0;
		virtual bool eof() = 0;
		virtual float getTiming() const = 0;

    public:
		virtual EVideoDecoderReadState readNextFrame( float _pts ) = 0;		
	};
}	// namespace Menge
