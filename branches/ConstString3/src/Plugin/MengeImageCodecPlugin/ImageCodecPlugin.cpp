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
////////////////////////////////////////////////////////////////////////////
//__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
//{
//	return initPluginMengeImageCodec( _plugin );
//}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecDecoderSystem::CodecDecoderSystem( const ConstString & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & CodecDecoderSystem::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecDecoderSystem::setService( CodecServiceInterface * _service )
	{
		m_service = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEncoderSystem::CodecEncoderSystem( const ConstString & _name )
		: m_name(_name)
		, m_service(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & CodecEncoderSystem::getName() const
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
			ImageDecoderSystem( const ConstString & _name, LogServiceInterface * _logService )
				: CodecDecoderSystem(_name)
				, m_logService(_logService)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{				
				return new T(m_service, _stream, m_logService);
			}

		protected:
			LogServiceInterface * m_logService;
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
			ImageEncoderSystem( const ConstString & _name, LogServiceInterface * _logService )
				: CodecEncoderSystem(_name)
				, m_logService(_logService)
			{
			}

		protected:
			EncoderInterface * createEncoder( OutputStreamInterface * _stream ) override
			{
				return new T(m_service, _stream, m_logService);
			}

		protected:
			LogServiceInterface * m_logService;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		m_codecService = _provider->getServiceT<CodecServiceInterface>( "CodecService" );

		if( m_codecService == 0 )
		{
			return;
		}

		LogServiceInterface * logService = _provider->getServiceT<LogServiceInterface>( "LogService" );
		
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderPNG>(ConstString("pngImage"), logService) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>(ConstString("jpegImage"), logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>(ConstString("jpgImage"), logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderMNE>("mneImage", logService) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderDDS>(ConstString("ddsImage"), logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderCombinerRGBAndAlpha>("combinedImage", logService) );
		
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderFFMPEG>("ffmpegVideo", logService) );

		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("oggVideo", logService) );
		//m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("ogvVideo", logService) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>(ConstString("oggSound"), logService) );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>(ConstString("ogvSound"), logService) );

		m_encoders.push_back( new Detail::ImageEncoderSystem<ImageEncoderPNG>(ConstString("pngImage"), logService) );

		//VideoDecoderOGGTheora::createCoefTables_();

		for( TVectorDecoders::iterator
			it = m_decoders.begin(),
			it_end = m_decoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_codecService->registerDecoder( name, (*it) );
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			const ConstString & name = (*it)->getName();
			m_codecService->registerEncoder( name, (*it) );
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
			const ConstString & name = (*it)->getName();
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
			const ConstString & name = (*it)->getName();
			m_codecService->unregisterEncoder( name );

			delete (*it);
		}

		delete this;
	}
}