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
		VideoCodecDataInfo()
			: frameWidth(0)
			, frameHeight(0)
			, fps(0)
			, frameTiming(0.f)
			, duration(0.f)
			, format(PF_UNKNOWN)
			, clamp(true)
		{
		}

		uint32_t frameWidth;
		uint32_t frameHeight;
		uint32_t frameWidthHW;
		uint32_t frameHeightHW;
		uint32_t fps;

		float frameTiming;		
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
		virtual bool seek( float _timing ) = 0;
		virtual float getTiming() const = 0;

    public:
        virtual void setPitch( size_t _pitch ) = 0;

    public:
		virtual EVideoDecoderReadState readNextFrame( float & _pts ) = 0;
	};

    typedef stdex::intrusive_ptr<VideoDecoderInterface> VideoDecoderInterfacePtr;
}
