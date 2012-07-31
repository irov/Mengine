#	include "VideoCodecDecoderSystem.h"

#	include "VideoDecoderFFMPEG.h"


#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif

extern "C"
{
#	include "libavformat/avformat.h"
#	include "libswscale/swscale.h"
#	include "libavcodec/avcodec.h"
#	include "libavutil/mathematics.h"
}

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecDecoderSystem::VideoCodecDecoderSystem( const ConstString & _name, LogServiceInterface * _logSystem )
		: m_name(_name)
		, m_logSystem(_logSystem)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & VideoCodecDecoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecDecoderSystem::initialize()
	{
		//input output interface
		avformat_network_init();

		// Register all formats and codecs
		av_register_all();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecDecoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * VideoCodecDecoderSystem::createDecoder( InputStreamInterface * _stream )
	{			
		DecoderInterface * decoder = new VideoDecoderFFMPEG(m_service, _stream, m_logSystem);

		return decoder;
	}

}