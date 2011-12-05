#	pragma once

#	include "Interface/CodecInterface.h"
#	include  "Interface/RenderSystemInterface.h"
namespace Menge
{
	struct VideoCodecDataInfo
		: public CodecDataInfo
	{
		std::size_t frame_width;
		std::size_t frame_height;
		float time_total_secs;
		
	};

	struct VideoCodecOptions
		: public CodecOptions
	{
		Menge::PixelFormat pixelFormat;
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
	};
}	// namespace Menge
