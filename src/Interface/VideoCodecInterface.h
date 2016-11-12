#	pragma once

#	include "Interface/CodecInterface.h"
#	include "Interface/RenderSystemInterface.h"

namespace Menge
{	
	enum EVideoDecoderReadState
	{
		VDRS_FAILURE = 0,
		VDRS_SUCCESS = 1,
		VDRS_END_STREAM = 3
	};

	struct VideoCodecDataInfo
		: public CodecDataInfo
	{
		VideoCodecDataInfo()
			: frameWidth(0)
			, frameHeight(0)
			, fps(0.f)
			, duration(0.f)
			, format(PF_UNKNOWN)
			, clamp(true)
		{
		}

		uint32_t frameWidth;
		uint32_t frameHeight;
		float fps;
		uint32_t channel;

		float duration;

		PixelFormat format;
		bool clamp;

		float getFrameTiming() const
		{
			return 1000.f / fps;
		}
	};
	

	struct VideoCodecOptions
		: public CodecOptions
	{
        VideoCodecOptions()
            : pixelFormat(PF_UNKNOWN)
			, duration(0.f)
			, fps(0.f)
			, alpha(false)
			, mock(false)
            , noSeek(false)
        {}

		PixelFormat pixelFormat;
		
		float duration;
		float fps;
		bool alpha;

		bool mock;
        bool noSeek;
	};

	class VideoDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual const VideoCodecDataInfo* getCodecDataInfo() const override = 0;

    public:
        virtual void setPitch( size_t _pitch ) = 0;

    public:
		virtual void updateFrame() = 0;
		virtual EVideoDecoderReadState readNextFrame( float & _pts ) = 0;
	};

    typedef stdex::intrusive_ptr<VideoDecoderInterface> VideoDecoderInterfacePtr;
}
