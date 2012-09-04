#	pragma once


#	include "Interface/VideoCodecInterface.h"

namespace Menge
{
	class VideoDecoder:
		public VideoDecoderInterface
	{
	public:
		VideoDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream );
		virtual ~VideoDecoder();
	public:
		const VideoCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( CodecOptions * _options ) override;

	protected:
		virtual void _invalidate();

	protected:
		void destroy() override;

    protected:
		CodecServiceInterface * m_service;
		InputStreamInterface * m_stream;
		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
