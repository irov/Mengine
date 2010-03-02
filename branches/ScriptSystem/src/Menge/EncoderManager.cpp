#	include "EncoderManager.h"
#	include "Encoder.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"

#	include "Utils.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * EncoderManager::createEncoder( const String& _fileSystemName, const String& _filename, const String& _type )
	{
		FileOutput* stream = FileEngine::hostage()
								->openFileOutput( _fileSystemName, _filename );

		EncoderInterface * encoder = this->createEncoder( _filename, _type, stream );

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * EncoderManager::createEncoder( const String& _filename, const String & _type, FileOutputInterface * _stream )
	{
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
		FileOutput * stream = _encoder->getStream();

		FileEngine::hostage()
			->closeFileOutput( stream );

		_encoder->destroy();
	}
}
