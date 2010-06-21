#	include "CodecEngine.h"
#	include "FileEngine.h"

#	include "Interface/CodecInterface.h"

#	include "Utils/Core/File.h"

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
	DecoderInterface * CodecEngine::createDecoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
	{
		FileInputInterface * stream = FileEngine::hostage()
			->createInputFile( _fileSystemName );

		DecoderInterface * decoder = 
			this->createDecoder( _filename, _type, stream );

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _filename, const ConstString& _type, FileInputInterface * _file )
	{
		bool res = _file->open( _filename );

		if( res == false )
		{
			return 0;
		}

		ConstString type = this->getType_( _filename, _type ); 

		DecoderInterface * decoder = 
			m_interface->createDecoder( type, _file );

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
	EncoderInterface * CodecEngine::createEncoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
	{
		FileOutputInterface * stream = FileEngine::hostage()
			->createOutputFile( _fileSystemName );

		EncoderInterface * encoder = 
			this->createEncoder( _filename, _type, stream );

		return encoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const String& _filename, const ConstString& _type, FileOutputInterface * _file )
	{
		ConstString type = this->getType_( _filename, _type ); 

		EncoderInterface * encoder = 
			m_interface->createEncoder( type, _file );

		return encoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseEncoder( EncoderInterface * _encoder )
	{
		FileOutputInterface * stream = _encoder->getStream();

		FileEngine::hostage()
			->closeOutputFile( stream );

		m_interface->releaseEncoder( _encoder );
	}
	//////////////////////////////////////////////////////////////////////////
	ConstString CodecEngine::getType_( const String& _filename, const ConstString& _type ) const
	{
		String typeExt;
		Utils::getFileExt( typeExt, _filename );

		typeExt += _type.str();

		return ConstManager::hostage()
			->genString( typeExt );
	}
}
