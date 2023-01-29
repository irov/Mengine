#include "TTFDataflow.h"

#include "TTFData.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static const char * FT_error_message( FT_Error err )
        {
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  case e: return s;
#define FT_ERROR_START_LIST     switch (err) {
#define FT_ERROR_END_LIST       }
#include FT_ERRORS_H
            return "(Unknown error)";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TTFDataflow::TTFDataflow()
        : m_library( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TTFDataflow::~TTFDataflow()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFDataflow::setFTLibrary( FT_Library _library )
    {
        m_library = _library;
    }
    //////////////////////////////////////////////////////////////////////////
    FT_Library TTFDataflow::getFTLibrary() const
    {
        return m_library;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFDataflow::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & TTFDataflow::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFDataflow::initialize()
    {
        m_factoryTTFData = Helper::makeFactoryPool<TTFData, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TTFDataflow::finalize()
    {
        m_mutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryTTFData );

        m_factoryTTFData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFDataflow::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr TTFDataflow::create( const DocumentPtr & _doc )
    {
        TTFDataPtr data = m_factoryTTFData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr TTFDataflow::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFDataflow::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        TTFData * data = _data.getT<TTFData *>();

        data->setTTFMemory( _memory );

        FT_Byte * memory_byte = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( memory_byte );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        FT_Face face;
        FT_Error err_code_memory_face = FT_New_Memory_Face( m_library, memory_byte, (FT_Long)memory_size, 0, &face );
        
        if( err_code_memory_face != FT_Err_Ok )
        {
            LOGGER_ERROR( "FT_New_Memory_Face font error: %s [%d] (doc: %s)"
                , Detail::FT_error_message( err_code_memory_face )
                , err_code_memory_face
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        FT_Error err_code_select_charmap = FT_Select_Charmap( face, FT_ENCODING_UNICODE );

        if( err_code_select_charmap != FT_Err_Ok )
        {
            LOGGER_ERROR( "FT_Select_Charmap font error: %s [%d] (doc: %s)"
                , Detail::FT_error_message( err_code_memory_face )
                , err_code_memory_face
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return false;
        }

        data->setFTFace( face );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}