#include "TTFDataflow.h"

#include "Interface/MemoryInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "TTFData.h"

#include "Kernel/ThreadMutexScope.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

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
    DataInterfacePtr TTFDataflow::create()
    {
        TTFDataPtr data = m_factoryTTFData->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFDataflow::load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        TTFData * data = stdex::intrusive_get<TTFData *>( _data );

        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, "TTFDataflow::load", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return false;
        }

        data->setTTFMemory( memory );

        FT_Byte * memory_byte = memory->getBuffer();
        size_t memory_size = memory->getSize();

        if( memory_byte == nullptr )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        FT_Face face;
        FT_Error err_code = FT_New_Memory_Face( m_library, memory_byte, (FT_Long)memory_size, 0, &face );

        if( err_code != 0 )
        {
            LOGGER_ERROR( "invalid FT_New_Memory_Face font"
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