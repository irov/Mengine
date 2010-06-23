#	include "CodecSystem.h"

#	include "Utils/Factory/FactoryDefault.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "VideoDecoderOGGTheora.h"
#	include "SoundDecoderOGGVorbis.h"

#	include "ImageEncoderPNG.h"

#	include "Utils/Core/File.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::CodecSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}

	*_interface = new Menge::CodecSystem();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::CodecSystemInterface* _interface )
{
	if( _interface )
	{
		delete static_cast<Menge::CodecSystem*>( _interface );
	}
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	bool CodecSystem::initialize()
	{
		m_decoderFactory.registerFactory( "pngImage", Helper::createFactoryDefault<ImageDecoderPNG>() );
		m_decoderFactory.registerFactory( "jpegImage", Helper::createFactoryDefault<ImageDecoderJPEG>() );
		m_decoderFactory.registerFactory( "jpgImage", Helper::createFactoryDefault<ImageDecoderJPEG>() );
		m_decoderFactory.registerFactory( "mneImage", Helper::createFactoryDefault<ImageDecoderMNE>() );

		m_decoderFactory.registerFactory( "oggVideo", Helper::createFactoryDefault<VideoDecoderOGGTheora>() );
		m_decoderFactory.registerFactory( "ogvVideo", Helper::createFactoryDefault<VideoDecoderOGGTheora>() );
		m_decoderFactory.registerFactory( "oggSound", Helper::createFactoryDefault<SoundDecoderOGGVorbis>() );
		m_decoderFactory.registerFactory( "ogvSound", Helper::createFactoryDefault<SoundDecoderOGGVorbis>() );

		// Encoders
		m_encoderFactory.registerFactory( "pngImage", Helper::createFactoryDefault<ImageEncoderPNG>() );

		VideoDecoderOGGTheora::createCoefTables_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecSystem::createDecoder( const String& _filename, const String& _type, FileInputInterface * _file )
	{
		Decoder * decoder = 
			m_decoderFactory.createObjectT<Decoder>( typeExt );

		if( decoder == 0 )
		{
			return 0;
		}

		decoder->initialize( _file, _type );

		if( decoder->getCodecDataInfo() == NULL )
		{
			releaseDecoder( decoder );
			return 0;
		}

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecSystem::releaseDecoder( DecoderInterface * _decoder )
	{
		Decoder * decoder = dynamic_cast<Decoder *>( _decoder );
		
		decoder->destroy();
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecSystem::createEncoder( const String& _type, FileOutputInterface * _stream )
	{
		if( _stream == 0 )
		{
			return 0;
		}

		Encoder * encoder = 
			m_encoderFactory.createObjectT<Encoder>( _type );

		if( encoder == NULL )
		{
			return NULL;
		}

		encoder->initialize( _stream, _type );

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecSystem::releaseEncoder( EncoderInterface * _encoder )
	{
		Encoder * encoder = dynamic_cast<Encoder *>( _encoder );

		encoder->destroy();
	}
}