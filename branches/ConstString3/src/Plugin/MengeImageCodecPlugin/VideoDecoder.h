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
		VideoDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream );

	public:
		const VideoCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( CodecOptions * _options ) override;

	protected:
		virtual void _invalidate();

	protected:
		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
