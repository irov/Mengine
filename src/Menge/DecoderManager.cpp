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
	Decoder * DecoderManager::createDecoder( const String& _filename, const String& _type )
	{
		DataStreamInterface* stream = Holder<FileEngine>::hostage()
			->openFile( _filename );

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
	void DecoderManager::releaseDecoder( Decoder * _decoder )
	{
		DataStreamInterface * stream = _decoder->getStream();
		
		Holder<FileEngine>::hostage()
			->closeStream( stream );

		_decoder->destructor();

		_decoder->destroy();
	}
}
