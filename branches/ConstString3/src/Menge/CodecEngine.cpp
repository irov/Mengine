#	include "CodecEngine.h"
#	include "FileEngine.h"

#	include "Interface/CodecInterface.h"

#	include "Utils/Core/File.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::CodecEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::~CodecEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerDecoder( const String& _type, DecoderSystemInterface * _interface )
	{
		ConstString ctype = ConstManager::get()
			->genString(_type);

		m_mapDecoderSystem.insert( std::make_pair(ctype, _interface) );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const String& _type, EncoderSystemInterface * _interface )
	{
		ConstString ctype = ConstManager::get()
			->genString(_type);

		m_mapEncoderSystem.insert( std::make_pair(ctype, _interface) );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
	{
		FileInputInterface * stream = FileEngine::get()
			->createInputFile( _fileSystemName );

		DecoderInterface * decoder = 
			this->createDecoder( _filename, _type, stream );

		return decoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const String& _filename, const ConstString& _type, FileInputInterface * _stream )
	{
		TMapDecoderSystem::iterator it_find = m_mapDecoderSystem.find( _type );

		if( it_find == m_mapDecoderSystem.end() )
		{
			return 0;
		}

		bool res = _stream->open( _filename );

		if( res == false )
		{
			return 0;
		}

		DecoderInterface * decoder = 
			it_find->second->createDecoder( _stream );

		if( decoder == 0 )
		{
			return 0;
		}

		if( decoder->initialize() == false )
		{
			this->releaseDecoder( decoder );
			return 0;
		}

		return decoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseDecoder( DecoderInterface * _decoder )
	{
		FileInputInterface * stream = _decoder->getStream();

		stream->close();
		_decoder->release();
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const ConstString& _fileSystemName, const String& _filename, const ConstString& _type )
	{
		FileOutputInterface * stream = FileEngine::get()
			->createOutputFile( _fileSystemName );

		EncoderInterface * encoder = 
			this->createEncoder( _filename, _type, stream );

		return encoder;		
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const String& _filename, const ConstString& _type, FileOutputInterface * _stream )
	{
		TMapEncoderSystem::iterator it_find = m_mapEncoderSystem.find( _type );

		if( it_find == m_mapEncoderSystem.end() )
		{
			return 0;
		}

		EncoderInterface * encoder = 
			it_find->second->createEncoder( _stream );

		if( encoder == 0 )
		{
			return 0;
		}

		if( encoder->initialize() == false )
		{
			this->releaseEncoder( encoder );
			return 0;
		}

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseEncoder( EncoderInterface * _encoder )
	{
		FileOutputInterface * stream = _encoder->getStream();

		FileEngine::get()
			->closeOutputFile( stream );

		_encoder->release();
	}
}
