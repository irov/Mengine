#	include "CodecEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"

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
		m_mapDecoderSystem.insert( std::make_pair(_type, _interface) );

		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterDecoder( const String& _type )
	{
		ConstString c_type(_type);
		m_mapDecoderSystem.erase( c_type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const String& _type, EncoderSystemInterface * _interface )
	{
		m_mapEncoderSystem.insert( std::make_pair(_type, _interface) );

		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterEncoder( const String& _type )
	{
		ConstString c_type(_type);
		m_mapEncoderSystem.erase( c_type );
	}
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::supportA8()
	{
		return RenderEngine::get()
			->supportA8();
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const char * _type, InputStreamInterface * _stream )
	{
		ConstString typeString ( _type );
		return createDecoder( typeString , _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const ConstString& _type, InputStreamInterface * _stream )
	{
		TMapDecoderSystem::iterator it_find = m_mapDecoderSystem.find( _type );
		
		if( it_find == m_mapDecoderSystem.end() )
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
			decoder->destroy();

			return 0;
		}

		return decoder;
	}
	//////////////////////////////////////////////////////////////////////////
	EncoderInterface * CodecEngine::createEncoder( const ConstString& _type, OutputStreamInterface * _stream )
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
			encoder->release();

			return 0;
		}

		return encoder;
	}
}
