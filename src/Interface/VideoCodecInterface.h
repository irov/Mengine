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
		size_t fps;
		float duration;

		PixelFormat format;
		bool clamp;
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
		virtual float getTiming() const = 0;

    public:
        virtual void setPitch( size_t _pitch ) = 0;

    public:
		virtual EVideoDecoderReadState readNextFrame( float & _pts ) = 0;
	};

    typedef stdex::intrusive_ptr<VideoDecoderInterface> VideoDecoderInterfacePtr;
}
