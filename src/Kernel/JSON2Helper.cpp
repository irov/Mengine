#include "JSON2Helper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

#include "json/json_dump.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {            
            //////////////////////////////////////////////////////////////////////////
            static void * __js_alloc_fun( size_t _size, void * _ud )
            {
                MENGINE_UNUSED( _ud );

                void * p = Helper::allocateMemory( _size, "json2" );

                return p;
            }
            //////////////////////////////////////////////////////////////////////////
            static void __js_free_fun( void * _ptr, void * _ud )
            {
                MENGINE_UNUSED( _ud );

                Helper::deallocateMemory( _ptr, "json2" );
            }
            //////////////////////////////////////////////////////////////////////////
            static void * __js_dump_buffer_data( js_size_t _size, void * _ud )
            {
                Data * data = reinterpret_cast<Data *>(_ud);

                Data::size_type size = data->size();

                data->resize( size + _size );

                Data::value_type * buffer = data->data();

                Data::value_type * buffer_write = buffer + size;

                return buffer_write;
            }
            //////////////////////////////////////////////////////////////////////////            
        }
        //////////////////////////////////////////////////////////////////////////
        js_element_t * createJSON2()
        {
            js_allocator_t allocator;
            js_make_allocator_default( &Detail::__js_alloc_fun, &Detail::__js_free_fun, JS_NULLPTR, &allocator );

            js_element_t * j;
            if( js_create( allocator, 0, &j ) == JS_FAILURE )
            {
                return nullptr;
            }

            return j;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroyJSON2( js_element_t * j )
        {
             js_free( j );
        }
        //////////////////////////////////////////////////////////////////////////
        bool writeJSON2DataCompact( js_element_t * _j, Data * const _data )
        {
            js_dump_ctx_t dump_ctx;
            js_make_dump_ctx_default( &Detail::__js_dump_buffer_data, _data, &dump_ctx );

            js_dump( _j, &dump_ctx );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
