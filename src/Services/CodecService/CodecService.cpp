#include "CodecService.h"

#include "Interface/CodecInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"

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
#if defined(MENGINE_DEBUG)
        for( auto && [key, value] : m_codecExts )
        {
            LOGGER_ERROR( "Codec exts '%s' not clear"
                , key.c_str()
            );
        }
#endif

        m_codecExts.clear();

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DecoderFactory" ) );
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "EncoderFactory" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderInterfacePtr CodecService::createDecoder( const ConstString & _type, const DocumentPtr & _doc )
    {
        DecoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "not found codec '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        DecoderInterfacePtr decoder = factory->createDecoder( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, nullptr );

        if( decoder->initialize() == false )
        {
            LOGGER_ERROR( "invalid initialize codec '%s' doc '%s'"
                , _type.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
    EncoderInterfacePtr CodecService::createEncoder( const ConstString & _type, const DocumentPtr & _doc )
    {
        EncoderFactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr, "not found codec '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
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
    void CodecService::removeCodecExt( const ConstString & _ext )
    {
        MENGINE_ASSERTION( m_codecExts.find( _ext ) != m_codecExts.end() );

        m_codecExts.erase( _ext );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & CodecService::findCodecType( const FilePath & _filePath ) const
    {
        ConstString ext = Helper::getFilePathExt( _filePath );

        MapCodecExts::const_iterator it_found = m_codecExts.find( ext );

        if( it_found == m_codecExts.end() )
        {
            return ConstString::none();
        }

        const ConstString & codecType = it_found->second;

        return codecType;
    }
}
