#	include "CodecEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _fileSystemName, const String& _filename, const String& _type )
	{
		FileInput* stream = FileEngine::hostage()
			->openFileInput( _fileSystemName, _filename );

		DecoderInterface * decoder = 
			m_interface->createDecoder( _filename, _type, stream );

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _filename, const String& _type, FileInputInterface * _file )
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
	void CodecEngine::releaseDecoder( DecoderInterface * _decoder )
	{
		FileInput * stream = _decoder->getStream();

		FileEngine::hostage()
			->closeFileInput( stream );

		m_interface->releaseDecoder( _decoder );
	}
}