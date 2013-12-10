#	include "CodecEngine.h"

#	include "Interface/CodecInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Utils/Core/File.h"

#	include "Logger/Logger.h"

#   include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CodecService, Menge::CodecServiceInterface, Menge::CodecEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CodecEngine::CodecEngine()
        : m_serviceProvider(nullptr)
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
	void CodecEngine::registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory )
	{
		m_mapDecoderSystem.insert( _type, _factory );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterDecoder( const ConstString& _type )
	{
		m_mapDecoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory )
	{
		m_mapEncoderSystem.insert( _type, _factory );
	}
	//////////////////////////////////////////////////////////////////////////
	void CodecEngine::unregisterEncoder( const ConstString& _type )
	{
		m_mapEncoderSystem.erase( _type );
	}
	//////////////////////////////////////////////////////////////////////////
	DecoderInterfacePtr CodecEngine::createDecoder( const ConstString& _type )
	{
		DecoderFactoryInterfacePtr decoderFactory;
		if( m_mapDecoderSystem.has_copy( _type, decoderFactory ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)("CodecEngine::createDecoder not found codec %s"
                , _type.c_str()
                );

			return nullptr;
		}        

		DecoderInterfacePtr decoder = decoderFactory->createDecoder();

		return decoder;
	}
    //////////////////////////////////////////////////////////////////////////
    EncoderInterfacePtr CodecEngine::createEncoder( const ConstString& _type )
    {
		EncoderFactoryInterfacePtr encoderSystem;
        if( m_mapEncoderSystem.has_copy( _type, encoderSystem ) == false )
        {
			LOGGER_ERROR(m_serviceProvider)("CodecEngine::createEncoder not found codec %s"
				, _type.c_str()
				);

            return nullptr;
        }

        EncoderInterfacePtr encoder = encoderSystem->createEncoder();

        return encoder;
    }
	//////////////////////////////////////////////////////////////////////////
	bool CodecEngine::registerCodecExt( const String & _ext, const ConstString & _codecType )
	{
		TMapCodecTypes::insert_type insert_ret = m_codecTypes.insert( _ext, _codecType );

		if( insert_ret.second == false )
		{
			const ConstString & alredyCodec = m_codecTypes.get_value( insert_ret.first );

			LOGGER_ERROR(m_serviceProvider)("CodecEngine::registerCodecExt '%s' '%s' alredy registry in '%s'"
				, _ext.c_str()
				, _codecType.c_str()
				, alredyCodec.c_str()
				);

			return false;
		}

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
            const String & ext = m_codecTypes.get_key( it );

            const char * str = _path.c_str();

            const char * dot_find = strrchr( str, '.' );

            if( dot_find == nullptr )
            {
                continue;
            }

            if( strcmp( dot_find + 1, ext.c_str() ) != 0 )
            {
                continue;
            }

            const ConstString & codecType = m_codecTypes.get_value( it );
            
            return codecType;
        }

        return ConstString::none();
	}
}
