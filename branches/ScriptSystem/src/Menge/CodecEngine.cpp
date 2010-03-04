#	include "CodecEngine.h"
#	include "FileEngine.h"

#	include "Interface/CodecInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::CodecEngine()
		: m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::~CodecEngine()
	{
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::initialize()
	{
		if( initInterfaceSystem( &m_interface ) == false )
		{
			return false;
		}

		bool result = m_interface->initialize();

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _fileSystemName, const String& _filename, const String& _type )
	{
		FileInputInterface * stream = FileEngine::hostage()
			->createFileInput( _fileSystemName );

		DecoderInterface * decoder = 
			this->createDecoder( _filename, _type, stream );

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _filename, const String& _type, FileInputInterface * _file )
	{
		DecoderInterface * decoder = 
			m_interface->createDecoder( _filename, _type, _file );

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseDecoder( DecoderInterface * _decoder )
	{
		FileInputInterface * stream = _decoder->getStream();

		FileEngine::hostage()
			->closeFileInput( stream );

		m_interface->releaseDecoder( _decoder );
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const String& _fileSystemName, const String& _filename, const String& _type )
	{
		FileOutputInterface * stream = FileEngine::hostage()
			->createFileOutput( _fileSystemName );

		EncoderInterface * encoder = 
			this->createEncoder( _filename, _type, stream );

		return encoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const String& _filename, const String& _type, FileOutputInterface * _file )
	{
		EncoderInterface * encoder = 
			m_interface->createEncoder( _filename, _type, _file );

		return encoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseEncoder( EncoderInterface * _encoder )
	{
		FileOutputInterface * stream = _encoder->getStream();

		FileEngine::hostage()
			->closeFileOutput( stream );

		m_interface->releaseEncoder( _encoder );
	}
}