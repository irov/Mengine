#include "JSONService.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"

#include "JSONStorage.h"

#include "jpp/jpp.hpp"

#include <string.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( JSONService, Mengine::JSONService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * my_jpp_malloc( size_t _size )
        {
            return Helper::allocateMemory( _size, "json" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void my_jpp_free( void * _ptr )
        {
            Helper::freeMemory( _ptr, "json" );
        }
        //////////////////////////////////////////////////////////////////////////
        static void my_jpp_error( int32_t _line, int32_t _column, int32_t _position, const char * _source, const char * _text, void * _ud )
        {
            MENGINE_UNUSED( _ud );

            LOGGER_ERROR( "jpp error: %s\nline: %d\n column: %d\nposition: %d\nsource: %s\n"
                , _text
                , _line
                , _column
                , _position
                , _source
            );
        }
        //////////////////////////////////////////////////////////////////////////
        struct my_json_load_data_t
        {
            const uint8_t * buffer;
            size_t carriage;
            size_t capacity;
        };
        //////////////////////////////////////////////////////////////////////////
        static size_t my_jpp_load_callback( void * _buffer, size_t _buflen, void * _data )
        {
            my_json_load_data_t * jd = static_cast<my_json_load_data_t *>(_data);

            if( _buflen > jd->capacity - jd->carriage )
            {
                _buflen = jd->capacity - jd->carriage;
            }

            if( _buflen <= 0 )
            {
                return 0;
            }

            const uint8_t * jd_buffer = jd->buffer + jd->carriage;
            ::memcpy( _buffer, jd_buffer, _buflen );
            jd->carriage += _buflen;

            return _buflen;            
        }
    }
    //////////////////////////////////////////////////////////////////////////
    JSONService::JSONService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONService::~JSONService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONService::_initializeService()
    {
        jpp::set_object_seed( 1 );

        jpp::set_alloc_funcs( &Detail::my_jpp_malloc, &Detail::my_jpp_free );

        m_factoryJSONStorage = Helper::makeFactoryPool<JSONStorage, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryJSONStorage );

        m_factoryJSONStorage = nullptr;

#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "json" );
        MENGINE_ASSERTION( report_count == 0, "json memleak [%d]"
            , report_count
        );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::createStorage( const jpp::object & _json, const DocumentPtr & _doc ) const
    {
        JSONStoragePtr storage = m_factoryJSONStorage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage, nullptr, "invalid create json storage" );

        storage->setJSON( _json );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) const
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

        JSONStorageInterfacePtr storage = this->loadJSONStream( stream, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, nullptr );

        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        JSONStorageInterfacePtr storage = this->createJSON( memory, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::createJSON( const MemoryInterfacePtr & _memory, const DocumentPtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _memory, nullptr );

        const void * memory_buffer = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        JSONStorageInterfacePtr storage = this->createJSONBuffer( memory_buffer, memory_size, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage, nullptr );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::createJSONBuffer( const void * _buffer, size_t _size, const DocumentPtr & _doc ) const
    {
        Detail::my_json_load_data_t jd;
        jd.buffer = static_cast<const uint8_t *>(_buffer);
        jd.carriage = 0;
        jd.capacity = _size;

        jpp::object json = jpp::load( &Detail::my_jpp_load_callback, &Detail::my_jpp_error, &jd );

        if( json == jpp::detail::invalid )
        {
            return nullptr;
        }

        JSONStoragePtr storage = this->createStorage( json, _doc );

        return storage;
    }
}

