#	include "CodecSystem.h"

#	include "Utils/Factory/FactoryDefault.h"

#	include "ImageDecoderPNG.h"
#	include "ImageDecoderJPEG.h"
#	include "ImageDecoderMNE.h"
#	include "ImageDecoderDDS.h"
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
		m_decoderFactory[ECT_IMAGE].registerFactory( "png", Helper::createFactoryDefault<ImageDecoderPNG>() );
		m_decoderFactory[ECT_IMAGE].registerFactory( "jpeg", Helper::createFactoryDefault<ImageDecoderJPEG>() );
		m_decoderFactory[ECT_IMAGE].registerFactory( "jpg", Helper::createFactoryDefault<ImageDecoderJPEG>() );
		m_decoderFactory[ECT_IMAGE].registerFactory( "mne", Helper::createFactoryDefault<ImageDecoderMNE>() );
		m_decoderFactory[ECT_IMAGE].registerFactory( "dds", Helper::createFactoryDefault<ImageDecoderDDS>() );

		m_decoderFactory[ECT_VIDEO].registerFactory( "ogg", Helper::createFactoryDefault<VideoDecoderOGGTheora>() );
		m_decoderFactory[ECT_VIDEO].registerFactory( "ogv", Helper::createFactoryDefault<VideoDecoderOGGTheora>() );

		m_decoderFactory[ECT_SOUND].registerFactory( "ogg", Helper::createFactoryDefault<SoundDecoderOGGVorbis>() );
		m_decoderFactory[ECT_SOUND].registerFactory( "ogv", Helper::createFactoryDefault<SoundDecoderOGGVorbis>() );

		// Encoders
		m_encoderFactory[ECT_IMAGE].registerFactory( "png", Helper::createFactoryDefault<ImageEncoderPNG>() );

		VideoDecoderOGGTheora::createCoefTables_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecSystem::createDecoder( const String& _filename, CodecType _type, FileInputInterface * _file )
	{
		bool res = _file->open( _filename );

		if( res == false )
		{
			return 0;
		}

		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		//typeExt += _type;

		Decoder * decoder = 
			m_decoderFactory[_type].createObjectT<Decoder>( typeExt );

		if( decoder == 0 )
		{
			return 0;
		}

		decoder->initialize( _file, typeExt );

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
	EncoderInterface * CodecSystem::createEncoder( const String& _filename, CodecType _type, FileOutputInterface * _stream )
	{
		if( _stream == 0 )
		{
			return 0;
		}

		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		//typeExt += _type;

		Encoder * encoder = 
			m_encoderFactory[_type].createObjectT<Encoder>( typeExt );

		if( encoder == NULL )
		{
			return NULL;
		}

		encoder->initialize( _stream, typeExt );

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecSystem::releaseEncoder( EncoderInterface * _encoder )
	{
		Encoder * encoder = dynamic_cast<Encoder *>( _encoder );

		encoder->destroy();
	}
}