#include "TTFDataflow.h"

#include "TTFData.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
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
        m_factoryTTFData = new FactoryPool<TTFData, 128>();

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
    DataInterfacePtr TTFDataflow::create( const Char * _doc )
    {
        TTFDataPtr data = m_factoryTTFData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr TTFDataflow::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFDataflow::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        TTFData * data = stdex::intrusive_get<TTFData *>( _data );

        data->setTTFMemory( _memory );

        FT_Byte * memory_byte = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        MENGINE_ASSERTION_MEMORY_PANIC( memory_byte, false );

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        FT_Face face;
        FT_Error err_code = FT_New_Memory_Face( m_library, memory_byte, (FT_Long)memory_size, 0, &face );

        if( err_code != 0 )
        {
            LOGGER_ERROR( "invalid FT_New_Memory_Face font (doc: %s)"
                , _doc
            );

            return false;
        }

        if( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
        {
            return false;
        }

        data->setFTFace( face );

        return true;
    }
}