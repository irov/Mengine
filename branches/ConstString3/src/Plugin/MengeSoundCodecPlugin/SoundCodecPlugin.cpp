#	include "SoundCodecPlugin.h"
#	include "SoundConverterFFMPEGToOGG.h"
#	include "Utils/Core/File.h"
#	include "Interface/LogSystemInterface.h"
#	include "Interface/StringizeInterface.h"

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
	SoundConverterSystem::SoundConverterSystem( const ConstString & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & SoundConverterSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundConverterSystem::setService( ConverterServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		template<class T>
		class SoundConverterFactory
			: public SoundConverterSystem
		{
		public:
			SoundConverterFactory( const ConstString & _name, FileServiceInterface * _fileService, LogServiceInterface * _logService, StringizeServiceInterface * _stringize )
				: SoundConverterSystem( _name )
				, m_logService( _logService )
				, m_fileService( _fileService )
				, m_stringize( _stringize )
			{
			}

		protected:
			ConverterInterface * createConverter( ) override
			{				
				return new T(m_service, m_fileService, m_logService, m_stringize);
			}

		protected:
			FileServiceInterface * m_fileService;
			LogServiceInterface * m_logService;
			StringizeServiceInterface * m_stringize;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	SoundCodecPlugin::SoundCodecPlugin()
		: m_converterService(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_converterService = _provider->getServiceT<ConverterServiceInterface>( "ConverterService" );

		if( m_converterService == 0 )
		{
			return false;
		}

		FileServiceInterface * fileService = _provider->getServiceT<FileServiceInterface>( "FileService" );
		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		StringizeServiceInterface * stringizeService = _provider->getServiceT<StringizeServiceInterface>( "StringizeService" );
		const ConstString & c_ffmpegToOgg = stringizeService->stringize("ffmpegToOggSound");

		m_converters.push_back( new Detail::SoundConverterFactory<SoundConverterFFMPEGToOGG>( c_ffmpegToOgg, fileService, logService, stringizeService));
		
		for( TVectorSoundConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_converterService->registerConverter( name, (*it) );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundCodecPlugin::finalize()
	{
		for( TVectorSoundConverters::iterator
			it = m_converters.begin(),
			it_end = m_converters.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_converterService->unregisterConverter( name );

			delete (*it);
		}

		delete this;
	}
}