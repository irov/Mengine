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
	void CodecEngine::registerEncoder( const String& _type, EncoderSystemInterface * _interface )
	{
		m_mapEncoderSystem.insert( std::make_pair(_type, _interface) );

		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::supportA8()
	{
		return RenderEngine::get()
			->supportA8();
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
			this->releaseDecoder( decoder );

			return 0;
		}

		return decoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseDecoder( DecoderInterface * _decoder )
	{
		_decoder->release();
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
			this->releaseEncoder( encoder );

			return 0;
		}

		return encoder;
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::releaseEncoder( EncoderInterface * _encoder )
	{
		_encoder->release();
	}
}
