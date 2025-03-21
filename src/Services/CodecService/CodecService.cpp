#include "CodecService.h"

#include "Interface/FactoryInterface.h"
#include "Interface/DecoderInterface.h"
#include "Interface/EncoderInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ThreadMutexHelper.h"

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
            LOGGER_ASSERTION( "codec exts '%s' not clear"
                , key.c_str()
            );
        }
#endif

        m_codecExts.clear();

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DecoderFactory" ) );
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "EncoderFactory" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    DecoderInterfacePointer CodecService::createDecoder( const ConstString & _type, const DocumentInterfacePtr & _doc )
    {
        FactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "DecoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not found decoder '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        DecoderInterfacePtr decoder = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, "invalid create decoder '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( _doc );

        if( decoder->initialize( mutex ) == false )
        {
            LOGGER_ERROR( "invalid initialize decoder '%s' doc '%s'"
                , _type.c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return nullptr;
        }

        return decoder;
    }
    //////////////////////////////////////////////////////////////////////////
    EncoderInterfacePointer CodecService::createEncoder( const ConstString & _type, const DocumentInterfacePtr & _doc )
    {
        FactoryInterfacePtr factory = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "EncoderFactory" ), _type );

        MENGINE_ASSERTION_MEMORY_PANIC( factory, "not found encoder '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        EncoderInterfacePtr encoder = factory->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( encoder, "invalid create encoder '%s' doc '%s'"
            , _type.c_str()
            , MENGINE_DOCUMENT_STR( _doc )
        );

        return encoder;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CodecService::registerCodecExt( const ConstString & _ext, const ConstString & _codecType )
    {
        MENGINE_ASSERTION( m_codecExts.find( _ext ) == m_codecExts.end(), "ext '%s' codec type '%s' already registry in '%s'"
            , _ext.c_str()
            , _codecType.c_str()
            , m_codecExts[_ext].c_str()
        );

        m_codecExts.emplace( _ext, _codecType );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CodecService::removeCodecExt( const ConstString & _ext )
    {
        MENGINE_ASSERTION( m_codecExts.find( _ext ) != m_codecExts.end(), "not found codec ext '%s'"
            , _ext.c_str()
        );

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
    //////////////////////////////////////////////////////////////////////////
}
