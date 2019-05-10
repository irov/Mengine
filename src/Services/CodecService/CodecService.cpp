#include "CodecService.h"

#include "Interface/CodecInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

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
    DecoderInterfacePtr CodecService::createDecoder( const ConstString & _type, const Char * _doc )
    {
        DecoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

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
        EncoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

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
