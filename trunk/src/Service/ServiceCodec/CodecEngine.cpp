#	include "CodecEngine.h"

#	include "Interface/CodecInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Utils/Core/File.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CodecService, Menge::CodecServiceInterface, Menge::CodecEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::CodecEngine()
        : m_serviceProvider(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::~CodecEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void CodecEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * CodecEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerDecoder( const ConstString& _type, DecoderFactoryInterface * _interface )
	{
		m_mapDecoderSystem.insert( std::make_pair(_type, _interface) );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterDecoder( const ConstString& _type )
	{
		m_mapDecoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const ConstString& _type, EncoderFactoryInterface * _interface )
	{
		m_mapEncoderSystem.insert( std::make_pair(_type, _interface) );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterEncoder( const ConstString& _type )
	{
		m_mapEncoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterface * CodecEngine::createDecoder( const ConstString& _type, InputStreamInterface * _stream )
	{
		TMapDecoderSystem::iterator it_find = m_mapDecoderSystem.find( _type );
		
		if( it_find == m_mapDecoderSystem.end() )
		{
            LOGGER_ERROR(m_serviceProvider)("CodecEngine::createDecoder not found codec %s"
                , _type.c_str()
                );

			return 0;
		}

        DecoderFactoryInterface * decoderFactory = it_find->second;

		DecoderInterface * decoder = decoderFactory->createDecoder();

		if( decoder == 0 )
		{
			return 0;
		}

		if( decoder->initialize( m_serviceProvider, _stream ) == false )
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

        EncoderFactoryInterface * encoderSystem = it_find->second;

        EncoderInterface * encoder = encoderSystem->createEncoder();

        if( encoder == 0 )
        {
            return 0;
        }

        if( encoder->initialize( m_serviceProvider, _stream ) == false )
        {
            encoder->destroy();

            return 0;
        }

        return encoder;
    }
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::registerCodecExt( const String & _ext, const ConstString & _codecType )
	{
		TMapCodecTypes::iterator it_find = m_codecTypes.find( _ext );

		if( it_find != m_codecTypes.end() )
		{
            const ConstString & alredyCodec = it_find->second;

			LOGGER_ERROR(m_serviceProvider)("CodecEngine::registerCodecExt '%s' '%s' alredy registry in '%s'"
				, _ext.c_str()
				, _codecType.c_str()
				, alredyCodec.c_str()
				);

			return false;
		}

		m_codecTypes.insert( std::make_pair(_ext, _codecType) );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & CodecEngine::findCodecType( const FilePath & _path ) const
	{
        for( TMapCodecTypes::const_iterator
            it = m_codecTypes.begin(),
            it_end = m_codecTypes.end();
        it != it_end;
        ++it )
        {
            const String & ext = it->first;

            const char * str = _path.c_str();

            const char * dot_find = strrchr( str, '.' );

            if( dot_find == NULL )
            {
                continue;
            }

            if( strcmp( dot_find + 1, ext.c_str() ) != 0 )
            {
                continue;
            }

            const ConstString & codecType = it->second;
            
            return codecType;
        }

        return ConstString::none();
	}
}
