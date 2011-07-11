#	include "ImageCodecPlugin.h"

#	include "Interface/CodecInterface.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "ImageDecoderDDS.h"

#	include "VideoDecoderOGGTheora.h"
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
			ImageDecoderSystem( const String & _name )
				: CodecDecoderSystem(_name)
			{
			}

		protected:
			DecoderInterface * createDecoder( InputStreamInterface * _stream ) override
			{
				return new T(m_service, _stream);
			}
		};

		template<class T>
		class ImageEncoderSystem
			: public CodecEncoderSystem
		{
		public:
			ImageEncoderSystem( const String & _name )
				: CodecEncoderSystem(_name)
			{
			}

		protected:
			EncoderInterface * createEncoder( OutputStreamInterface * _stream ) override
			{
				return new T(m_service, _stream);
			}
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void ImageCodecPlugin::initialize( ServiceProviderInterface * _provider )
	{
		ServiceInterface * service = _provider->getService( "Codec" );

		if( service == 0 )
		{
			return;
		}

		m_codecService = static_cast<CodecServiceInterface*>(service);
	}

	void ImageCodecPlugin::run( const TMapParam & _params )
	{
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderPNG>("pngImage") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>("jpegImage") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderJPEG>("jpgImage") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderMNE>("mneImage") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<ImageDecoderDDS>("ddsImage") );

		m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("oggVideo") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<VideoDecoderOGGTheora>("ogvVideo") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>("oggSound") );
		m_decoders.push_back( new Detail::ImageDecoderSystem<SoundDecoderOGGVorbis>("ogvSound") );

		m_encoders.push_back( new Detail::ImageEncoderSystem<ImageEncoderPNG>("pngImage") );

		VideoDecoderOGGTheora::createCoefTables_();

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
			m_codecService->unregisterDecoder( (*it)->getName() );
			delete (*it);
		}

		// Encoders
		for( TVectorEncoders::iterator
			it = m_encoders.begin(),
			it_end = m_encoders.end();
		it != it_end;
		++it )
		{
			m_codecService->unregisterEncoder( (*it)->getName() );
			delete (*it);
		}

		delete this;
	}
}