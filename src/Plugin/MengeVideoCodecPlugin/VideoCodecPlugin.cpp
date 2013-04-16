#	include "VideoCodecPlugin.h"

#   include "VideoDecoderFFMPEG.h"

#	include "Core/File.h"

#   include "Codec/DecoderFactory.h"

#   include "Interface/StringizeInterface.h"


#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif

#pragma warning(push, 0) 
extern "C"
{
#	include "libavformat/avformat.h"
#	include "libswscale/swscale.h"
#	include "libavcodec/avcodec.h"
#	include "libavutil/mathematics.h"
}
#pragma warning(pop) 

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin )
	{
		*_plugin = new Menge::VideoCodecPlugin();
		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
	{
		return initPluginMengeVideoCodec( _plugin );
	}
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static void s_ffmpeg_log( void* _impl, int _level, const char* _format, va_list _args )
    {
        (void)_impl;

        if( _level > AV_LOG_WARNING )
        {
            return;
        }

        vprintf( _format, _args );
    }
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
		: m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool VideoCodecPlugin::initialize( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

        avcodec_register_all();
        av_register_all();

        av_log_set_callback( &s_ffmpeg_log );

		m_decoders.push_back( new DecoderFactory<VideoDecoderFFMPEG>(m_serviceProvider, Helper::stringizeString(m_serviceProvider, "ffmpegVideo")) );
		
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{			
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)->registerDecoder( name, (*it) );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::destroy()
	{
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();

			CODEC_SERVICE(m_serviceProvider)->unregisterDecoder( name );

			(*it)->destroy();
		}

		delete this;
	}
}