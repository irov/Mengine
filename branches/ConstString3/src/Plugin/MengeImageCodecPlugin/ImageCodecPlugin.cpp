#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"
#	include "Interface/LogSystemInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "ImageDecoderDDS.h"
#   include "ImageDecoderCombinerRGBAndAlpha.h"
//#	include "VideoDecoderFFMPEG.h"

//#	include "VideoDecoderOGGTheora.h"
#	include "SoundDecoderOGGVorbis.h"

#	include "ImageEncoderPNG.h"

#	include "Utils/Core/File.h"

//////////////////////////////////////////////////////////////////////////
bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::ImageCodecPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecDecoderSystem::CodecDecoderSystem( const String & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & CodecDecoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecDecoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEncoderSystem::CodecEncoderSystem( const String & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & CodecEncoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEncoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace Detail
	{
		template<class T>
		class ImageDecoderSystem
			: public CodecDecoderSystem
		{
		public:
			ImageDecoderSystem( const String & _name, LogSystemInterface * _logSystem )
				: CodecDecoderSystem(_name)
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
		/*
		template<class T>
		class ImageDecoderCombinerSystem
			: public CodecDecoderSystem
		{
		public:
			ImageDecoderCombinerSystem( const String & _name, LogSystemInterface * _logSystem )
				: CodecDecoderSystem(_name)
				, m_logSystem(_logSystem)
			{
			}

		protected:
			virtual DecoderInterface * createDecoderCombiner( ImageDecoderInterface * _decoderFirst, ImageDecoderInterface * _decoderSecond ) 
			{				
				return new T( _decoderFirst, _decoderSecond, m_logSystem );
			}
			
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{				
				return NULL;
			}
		protected:
			LogSystemInterface * m_logSystem;
		};
		*/

		template<class T>
		class ImageEncoderSystem
			: public CodecEncoderSystem
		{
		public:
			ImageEncoderSystem( const String & _name, LogSystemInterface * _logSystem )
				: CodecEncoderSystem(_name)
				, m_logSystem(_logSystem)
			{
			}

		protected:
			EncoderInterface * createEncoder( OutputStreamInterface * _stream ) override
			{
				return new T(m_service, _stream, m_logSystem);
			}

		protected:
			LogSystemInterface * m_logSystem;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "Codec" );

		if( m_codecService == 0 )
		{
			return;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "Log" );

		LogSystemInterface * logSystem = logService->getInterface();

		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderPNG>("pngImage", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>("jpegImage", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>("jpgImage", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderMNE>("mneImage", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderDDS>("ddsImage", logSystem) );
		//m_decoders.push_back( new Detail::ImageDecoderCombinerSystem<ImageDecoderCombinerRGBAndAlpha>("combinedImage", logSystem) );
		
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderFFMPEG>("Video", logSystem) );

		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("oggVideo", logSystem) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("ogvVideo", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>("oggSound", logSystem) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>("ogvSound", logSystem) );

		m_encoders.push_back( new Detail::ImageEncoderSystem<ImageEncoderPNG>("pngImage", logSystem) );

		//VideoDecoderOGGTheora::createCoefTables_();

		for( TVectorDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			m_codecService->registerDecoder( (*it)->getName(), (*it) );
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			m_codecService->registerEncoder( (*it)->getName(), (*it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::finalize()
	{
		for( TVectorDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const String & name = (*it)->getName();
			m_codecService->unregisterDecoder( name );

			delete (*it);
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			const String & name = (*it)->getName();
			m_codecService->unregisterEncoder( name );

			delete (*it);
		}

		delete this;
	}
}