#	include "Codec.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"

#	include "DecoderImplement.h"
#	include "EncoderImplement.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	void DecoderManager::registerDecoder( const String& _type, Factory::TGenFunc _creator )
	{
		m_factory.registration( _type, _creator );
	}
	//////////////////////////////////////////////////////////////////////////
	void DecoderManager::unregisterDecoder( const String& _type )
	{
		m_factory.unregistration( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * DecoderManager::createDecoder( const String& _filename, const String& _type )
	{
		String strExt;

		std::size_t pos = _filename.find_last_of( "." );

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		DataStreamInterface* stream = Holder<FileEngine>::hostage()
			->openFile( _filename );

		if( stream == 0 )
		{
			return 0;
		}

		DecoderDesc desc;

		desc.stream = stream;
		desc.type = strExt + _type;

		DecoderInterface * decoderInterface = m_factory.generate_t<DecoderInterface>( desc.type, desc );

		if( decoderInterface == 0 )
		{
			return 0;
		}

		if( decoderInterface->getCodecDataInfo() == NULL )
		{
			releaseDecoder( decoderInterface );
			return 0;
		}

		return decoderInterface;		
	}
	//////////////////////////////////////////////////////////////////////////
	void DecoderManager::releaseDecoder( DecoderInterface* _decoder )
	{
		//_codec->release();
		Holder<FileEngine>::hostage()
			->closeStream( _decoder->getStream() );

		_decoder->destructor();

		delete _decoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void EncoderManager::registerEncoder( const String& _type, Factory::TGenFunc _creator )
	{
		m_factory.registration( _type, _creator );
	}
	//////////////////////////////////////////////////////////////////////////
	void EncoderManager::unregisterEncoder( const String& _type )
	{
		m_factory.unregistration( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * EncoderManager::createEncoder( const String& _filename, const String& _type )
	{
		String strExt;

		std::size_t pos = _filename.find_last_of( "." );

		while( pos != _filename.length() - 1 )
		{
			strExt += _filename[++pos];
		}

		OutStreamInterface* stream = Holder<FileEngine>::hostage()
			->openOutStream( _filename, true );

		EncoderDesc desc;

		desc.stream = stream;
		desc.type = strExt + _type;

		EncoderInterface * encoderInterface = m_factory.generate_t<EncoderInterface>( desc.type, desc );

		return encoderInterface;
	}

	void EncoderManager::releaseEncoder( EncoderInterface* _encoder )
	{
		Holder<FileEngine>::hostage()
			->closeOutStream( _encoder->getStream() );

		_encoder->destructor();
	}
} // namespace Menge
