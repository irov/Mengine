#include "JSONService.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

#include "jansson.h"

#include "jpp/jpp.hpp"

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
        json_object_seed( 1 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void JSONService::_finalizeService()
    {
#ifdef STDEX_ALLOCATOR_REPORT_ENABLE
        uint32_t report_count = stdex_get_allocator_report_count( "json" );
        MENGINE_ASSERTION( report_count == 0, "json memleak [%d]"
            , report_count
        );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONService::loadJSON( const InputStreamInterfacePtr & _stream, jpp::object * _json, const Char * _doc ) const
    {
        MemoryInterfacePtr memory = Helper::createMemoryStream( _stream, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        Detail::my_json_load_data_t jd;
        jd.buffer = static_cast<const uint8_t *>(memory_buffer);
        jd.carriage = 0;
        jd.capacity = memory_size;

        jpp::object json = jpp::load( &Detail::my_jpp_load_callback, &Detail::my_jpp_malloc, &Detail::my_jpp_free, &Detail::my_jpp_error, &jd );

        if( json == jpp::detail::invalid )
        {
            return false;
        }

        *_json = json;

        return true;
    }
}

