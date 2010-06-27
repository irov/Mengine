#	include "VideoDecoder.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoDecoder::VideoDecoder( FileInputInterface * _stream )
		: Decoder(_stream)
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
}
