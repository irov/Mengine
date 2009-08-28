#	include "EncoderManager.h"
#	include "Encoder.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"

#	include "Utils.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	Encoder * EncoderManager::createEncoder( const String& _filename, const String& _type )
	{
		OutStreamInterface* stream = Holder<FileEngine>::hostage()
			->openOutStream( _filename, true );

		if( stream == 0 )
		{
			return 0;
		}

		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		typeExt += _type;

		Encoder * encoder = 
			FactoryManager::createObjectT<Encoder>( typeExt );

		if( encoder == NULL )
		{
			return NULL;
		}

		encoder->initialize( stream, typeExt );

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void EncoderManager::releaseEncoder( Encoder* _encoder )
	{
		Holder<FileEngine>::hostage()
			->closeOutStream( _encoder->getStream() );

		_encoder->destructor();

		_encoder->destroy();
	}
}
