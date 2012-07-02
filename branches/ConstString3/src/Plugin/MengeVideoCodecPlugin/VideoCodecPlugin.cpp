#	include "VideoCodecPlugin.h"
#	include "VideoCodecDecoderSystem.h"

#	include "VideoDecoderFFMPEG.h"

#	include "Utils/Core/File.h"

#	include "Interface/LogSystemInterface.h"

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
	namespace Detail
	{
		template<class T>
		class VideoDecoderSystem
			: public VideoCodecDecoderSystem
		{
		public:
			VideoDecoderSystem( const ConstString & _name, LogServiceInterface * _logSystem )
				: VideoCodecDecoderSystem(_name)
				, m_logSystem(_logSystem)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{				
				return new T(m_service, _stream, m_logSystem);
			}

		protected:
			LogServiceInterface * m_logSystem;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
		: m_codecService(0)
		, m_stringize(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "CodecService" );
		m_stringize = _provider->getServiceT<StringizeServiceInterface>( "StringizeService" );

		if( m_codecService == 0 )
		{
			return;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );
		
		ConstString c_ffmpegVideo =  m_stringize->stringize("ffmpegVideo");

		VideoCodecDecoderSystem * ffmpegDecoder = new Detail::VideoDecoderSystem<VideoDecoderFFMPEG>(c_ffmpegVideo, logService);
		m_decoders.push_back( ffmpegDecoder );
		
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{			
			const ConstString & name = (*it)->getName();
			m_codecService->registerDecoder( name, (*it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::finalize()
	{
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_codecService->unregisterDecoder( name );

			delete (*it);
		}

		delete this;
	}
}