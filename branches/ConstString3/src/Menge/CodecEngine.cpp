#	include "CodecEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"

#	include "Interface/CodecInterface.h"

#	include "Utils/Core/File.h"

#	include "LogEngine.h"

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
	void CodecEngine::registerDecoder( const ConstString& _type, DecoderSystemInterface * _interface )
	{
		m_mapDecoderSystem.insert( std::make_pair(_type, _interface) );

		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterDecoder( const ConstString& _type )
	{
		ConstString c_type(_type);
		m_mapDecoderSystem.erase( c_type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const ConstString& _type, EncoderSystemInterface * _interface )
	{
		m_mapEncoderSystem.insert( std::make_pair(_type, _interface) );

		_interface->setService( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterEncoder( const ConstString& _type )
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
	bool CodecEngine::registerCodecExt( const WString & _ext, const ConstString & _codecType )
	{
		TMapCodecTypes::iterator it_find = m_codecTypes.find( _ext );

		if( it_find != m_codecTypes.end() )
		{
			MENGE_LOG_ERROR("CodecEngine::registerCodecExt '%S' '%s' alredy registry in '%s'"
				, _ext.c_str()
				, _codecType.c_str()
				, it_find->second.c_str()
				);

			return false;
		}

		m_codecTypes.insert( std::make_pair(_ext, _codecType) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & CodecEngine::findCodecType( const WString & _ext )
	{
		TMapCodecTypes::iterator it_find = m_codecTypes.find( _ext );

		if( it_find == m_codecTypes.end() )
		{
			MENGE_LOG_WARNING("CodecEngine::findCodecType codec for '%S' not found"
				, _ext.c_str()
				);

			return ConstString::none;
		}

		const ConstString & codecType = it_find->second;

		return codecType;
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
