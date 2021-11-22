#include "JSONService.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"

#include "JSONStorage.h"

#include "Config/StdString.h"

#include "jpp/jpp.hpp"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( JSONService, Mengine::JSONService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
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
            MENGINE_MEMCPY( _buffer, jd_buffer, _buflen );
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
        m_factoryJSONStorage = Helper::makeFactoryPool<JSONStorage, 16>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryJSONStorage );

        m_factoryJSONStorage = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::createStorage( const jpp::object & _json, bool _copy, const DocumentPtr & _doc ) const
    {
        JSONStoragePtr storage = m_factoryJSONStorage->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage, "invalid create json storage" );

        if( _copy == true )
        {
            jpp::object copy_json = jpp::copy( _json );

            storage->setJSON( copy_json );
        }
        else
        {
            storage->setJSON( _json );
        }

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::copyStorage( const JSONStorageInterfacePtr & _storage, const DocumentPtr & _doc ) const
    {
        const jpp::object & json = _storage->getJSON();

        JSONStorageInterfacePtr storage = this->createStorage( json, true, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) const
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        JSONStorageInterfacePtr storage = this->loadJSONStream( stream, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream );

        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        JSONStorageInterfacePtr storage = this->loadJSONStreamFromMemory( memory, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSONStreamFromMemory( const MemoryInterfacePtr & _memory, const DocumentPtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _memory );

        const void * memory_buffer = _memory->getBuffer();
        size_t memory_size = _memory->getSize();

        JSONStorageInterfacePtr storage = this->loadJSONStreamFromBuffer( memory_buffer, memory_size, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( storage );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
    JSONStorageInterfacePtr JSONService::loadJSONStreamFromBuffer( const void * _buffer, size_t _size, const DocumentPtr & _doc ) const
    {
        Detail::my_json_load_data_t jd;
        jd.buffer = static_cast<const uint8_t *>(_buffer);
        jd.carriage = 0;
        jd.capacity = _size;

        jpp::object json = jpp::load( &Detail::my_jpp_load_callback, jpp::JPP_LOAD_MODE_DISABLE_EOF_CHECK, &Detail::my_jpp_error, &jd );

        if( json == jpp::detail::invalid )
        {
            return nullptr;
        }

        JSONStoragePtr storage = this->createStorage( json, false, _doc );

        return storage;
    }
    //////////////////////////////////////////////////////////////////////////
}

