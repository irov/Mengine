#	include "VideoCodecPlugin.h"
#	include "VideoDecoderFFMPEG.h"
#	include "Utils/Core/File.h"
#	include "Interface/LogSystemInterface.h"
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::VideoCodecPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VideoCodecDecoderSystem::VideoCodecDecoderSystem( const String & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & VideoCodecDecoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecDecoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		template<class T>
		class VideoDecoderSystem
			: public VideoCodecDecoderSystem
		{
		public:
			VideoDecoderSystem( const String & _name, LogSystemInterface * _logSystem )
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
			LogSystemInterface * m_logSystem;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	VideoCodecPlugin::VideoCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "Codec" );

		if( m_codecService == 0 )
		{
			return;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "Log" );

		LogSystemInterface * logSystem = logService->getInterface();
		m_decoders.push_back( new Detail::VideoDecoderSystem<VideoDecoderFFMPEG>("ffmpegVideo", logSystem));
		
		for( TVectorVideoDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			
			m_codecService->registerDecoder( (*it)->getName(), (*it) );
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
			const String & name = (*it)->getName();
			m_codecService->unregisterDecoder( name );

			delete (*it);
		}
		delete this;
	}
}