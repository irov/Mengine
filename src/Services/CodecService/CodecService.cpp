#include "CodecService.h"

#include "Interface/CodecInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"

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
    bool CodecService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CodecService::_finalizeService()
    {
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DecoderFactory" ) );
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "EncoderFactory" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderInterfacePtr CodecService::createDecoder( const ConstString & _type, const Char * _doc )
    {
        DecoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "not found codec '%s' doc '%s'"
            , _type.c_str()
            , _doc
        );

        DecoderInterfacePtr decoder = factory->createDecoder( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, nullptr );

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
    EncoderInterfacePtr CodecService::createEncoder( const ConstString & _type, const Char * _doc )
    {
        EncoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "not found codec '%s' doc '%s'"
            , _type.c_str()
            , _doc
        );

        EncoderInterfacePtr encoder = factory->createEncoder( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, nullptr );

        return encoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CodecService::registerCodecExt( const ConstString & _ext, const ConstString & _codecType )
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

        ConstString ext = Helper::stringizeString( dot_find + 1 );

        MapCodecExts::const_iterator it_found = m_codecExts.find( ext );

        if( it_found == m_codecExts.end() )
        {
            return ConstString::none();
        }

        const ConstString & codecType = it_found->second;

        return codecType;
    }
}
