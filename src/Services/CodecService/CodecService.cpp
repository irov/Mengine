#include "CodecService.h"

#include "Interface/CodecInterface.h"

#include "Kernel/Logger.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CodecService, Mengine::CodecService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CodecService::CodecService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CodecService::~CodecService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CodecService::registerDecoder( const ConstString& _type, const DecoderFactoryInterfacePtr & _factory )
    {
        m_factorDecoders.insert( _type, _factory );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderFactoryInterfacePtr CodecService::unregisterDecoder( const ConstString& _type )
    {
        DecoderFactoryInterfacePtr remove_decoder = m_factorDecoders.remove( _type );

        return remove_decoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CodecService::registerEncoder( const ConstString& _type, const EncoderFactoryInterfacePtr & _factory )
    {
        m_factorEncoders.insert( _type, _factory );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    EncoderFactoryInterfacePtr CodecService::unregisterEncoder( const ConstString& _type )
    {
        EncoderFactoryInterfacePtr remove_encoder = m_factorEncoders.remove( _type );

        return remove_encoder;
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderInterfacePtr CodecService::createDecoder( const ConstString & _type, const Char * _doc )
    {
        const DecoderFactoryInterfacePtr & factory = m_factorDecoders.find( _type );

        if( factory == nullptr )
        {
            LOGGER_ERROR( "not found codec '%s' doc '%s'"
                , _type.c_str()
                , _doc
            );

            return nullptr;
        }

        DecoderInterfacePtr decoder = factory->createDecoder( _doc );

        if( decoder->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize codec '%s' doc '%s'"
                , _type.c_str()
                , _doc
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
    EncoderInterfacePtr CodecService::createEncoder( const ConstString& _type, const Char * _doc )
    {
        const EncoderFactoryInterfacePtr & factory = m_factorEncoders.find( _type );

        if( factory == nullptr )
        {
            LOGGER_ERROR( "not found codec '%s' doc '%s'"
                , _type.c_str()
                , _doc
            );

            return nullptr;
        }

        EncoderInterfacePtr encoder = factory->createEncoder( _doc );

        return encoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CodecService::registerCodecExt( const String & _ext, const ConstString & _codecType )
    {
        MapCodecExts::iterator it_found = m_codecExts.find( _ext );

        if( it_found != m_codecExts.end() )
        {
            const ConstString & alredyCodec = it_found->second;

            LOGGER_ERROR( "ext '%s' codec type '%s' alredy registry in '%s'"
                , _ext.c_str()
                , _codecType.c_str()
                , alredyCodec.c_str()
            );

            return false;
        }

        m_codecExts.emplace( _ext, _codecType );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & CodecService::findCodecType( const FilePath & _path ) const
    {
        const Char * str_path = _path.c_str();

        const Char * dot_find = strrchr( str_path, '.' );

        if( dot_find == nullptr )
        {
            return ConstString::none();
        }

        String ext( dot_find + 1 );

        MapCodecExts::const_iterator it_found = m_codecExts.find( ext );

        if( it_found == m_codecExts.end() )
        {
            return ConstString::none();
        }

        const ConstString & codecType = it_found->second;

        return codecType;
    }
}
