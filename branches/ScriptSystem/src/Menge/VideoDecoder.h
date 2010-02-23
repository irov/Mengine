#	pragma once

#	include "Decoder.h"

#	include "Interface/VideoCodecInterface.h"

namespace Menge
{
	class VideoDecoder
		: public Decoder
		, public VideoDecoderInterface
	{
	public:
		VideoDecoder();

	public:
		const VideoCodecDataInfo * getCodecDataInfo() const override;

	protected:
		VideoCodecDataInfo m_dataInfo;
	};
}
