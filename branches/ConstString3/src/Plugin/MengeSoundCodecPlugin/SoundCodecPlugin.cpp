#	include "SoundCodecPlugin.h"
#	include "SoundDecoderConverterFFMPEGToOGG.h"
#	include "Utils/Core/File.h"
#	include "Interface/LogSystemInterface.h"
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::SoundCodecPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundCodecDecoderSystem::SoundCodecDecoderSystem( const String & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & SoundCodecDecoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecDecoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		template<class T>
		class SoundDecoderSystem
			: public SoundCodecDecoderSystem
		{
		public:
			SoundDecoderSystem( const String & _name, LogSystemInterface * _logSystem ,FileServiceInterface * _fileService)
				: SoundCodecDecoderSystem(_name)
				, m_logSystem(_logSystem)
				, m_fileService( _fileService )
			{
			}
		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{				
				return new T(m_service, _stream, m_logSystem, m_fileService);
			}
		protected:
			FileServiceInterface * m_fileService;
			LogSystemInterface * m_logSystem;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	SoundCodecPlugin::SoundCodecPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "Codec" );

		if( m_codecService == 0 )
		{
			return;
		}

		FileServiceInterface * fileService = _provider->getServiceT<FileServiceInterface>( "File" );
		
		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "Log" );

		LogSystemInterface * logSystem = logService->getInterface();
		
		m_decoders.push_back( new Detail::SoundDecoderSystem<SoundDecoderConverterFFMPEGToOGG>("ffmpegToOggSound", logSystem, fileService));
		
		for( TVectorSoundDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			m_codecService->registerDecoder( (*it)->getName(), (*it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::finalize()
	{
		for( TVectorSoundDecoders::iterator
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