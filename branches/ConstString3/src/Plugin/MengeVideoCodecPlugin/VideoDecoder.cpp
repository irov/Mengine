#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder( CodecServiceInterface * _service, InputStreamInterface * _stream )
		: m_service(_service)
		, m_stream(_stream)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const VideoCodecDataInfo * VideoDecoder::getCodecDataInfo() const 
	{
		return &m_dataInfo;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoder::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<VideoCodecOptions*>(_options);

		this->_invalidate();
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoder::_invalidate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoDecoder::destroy()
	{
		delete this;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface * VideoDecoder::getStream() const
	{
		return m_stream;	
	}
}
