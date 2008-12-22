
#	pragma once

#	include "Interface/CodecInterface.h"

namespace Menge
{

	struct VideoCodecDataInfo
		: public CodecDataInfo
	{
		std::size_t frame_width;
		std::size_t frame_height;
		float time_total_secs;
	};

	class VideoDecoderInterface
		: public DecoderInterface
	{
	public:
		virtual int sync( float _timing ) = 0;
		virtual bool seek( float _timing ) = 0;
	};
}	// namespace Menge