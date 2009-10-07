#	include "DecoderManager.h"

#	include "Decoder.h"

#	include "Interface/ImageCodecInterface.h"

#	include "FileEngine.h"

//#	include "FactorableImplement.h"
//#	include "EncoderImplement.h"

#	include "Utils.h"

namespace Menge 
{
	//////////////////////////////////////////////////////////////////////////
	Decoder * DecoderManager::createDecoder( const String& _fileSystemName, const String& _filename, const String& _type )
	{
		FileInput* stream = FileEngine::hostage()
								->openFileInput( _fileSystemName, _filename );

		if( stream == 0 )
		{
			return 0;
		}
		
		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		typeExt += _type;

		Decoder * decoder = 
			this->createObjectT<Decoder>( typeExt );

		if( decoder == 0 )
		{
			return 0;
		}

		decoder->initialize( stream, typeExt );

		if( decoder->getCodecDataInfo() == NULL )
		{
			releaseDecoder( decoder );
			return 0;
		}

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	Decoder * DecoderManager::createDecoder( const String& _filename, const String& _type, FileInput* _file )
	{
		bool res = _file->open( _filename );

		if( res == false )
		{
			return 0;
		}


		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		typeExt += _type;

		Decoder * decoder = 
			this->createObjectT<Decoder>( typeExt );

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
	void DecoderManager::releaseDecoder( Decoder * _decoder )
	{
		FileInput* stream = _decoder->getStream();
		
		FileEngine::hostage()
			->closeFileInput( stream );

		_decoder->destructor();

		_decoder->destroy();
	}
}
