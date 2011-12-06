#	pragma once


#	include "Interface/VideoCodecInterface.h"

namespace Menge
{
	class VideoDecoder:
		public VideoDecoderInterface
	{
	public:
		VideoDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream );

	public:
		const VideoCodecDataInfo * getCodecDataInfo() const override;
		void setOptions( CodecOptions * _options ) override;

	protected:
		virtual void _invalidate();
		virtual InputStreamInterface * getStream() const;
	protected:
		virtual void destroy();
		CodecServiceInterface * m_service;
		InputStreamInterface * m_stream;
		VideoCodecOptions m_options;
		VideoCodecDataInfo m_dataInfo;
	};
}
