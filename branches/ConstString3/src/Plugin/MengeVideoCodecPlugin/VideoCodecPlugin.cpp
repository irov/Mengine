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
	VideoCodecDecoderSystem::VideoCodecDecoderSystem( const ConstString & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & VideoCodecDecoderSystem::getName() const
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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void VideoCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
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