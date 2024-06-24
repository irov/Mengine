#include "StaticString.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVSNPRINTF.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        size_t staticStringSize( const Char * _buffer, size_t _capacity )
        {
            MENGINE_UNUSED( _capacity );

            size_t buffer_size = MENGINE_STRLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size < _capacity, "static string buffer '%s' [size %zu] > [capacity %zu]"
                , _buffer
                , buffer_size
                , _capacity
            );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        Char staticStringIndex( const Char * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity, "static string buffer index [index %zu] > [capacity %zu]"
                , _index
                , _capacity
            );

            Char c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringChange( Char * const _buffer, size_t _capacity, size_t _index, Char _ch )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity, "static string buffer change index [index %zu] > [capacity %zu]"
                , _index
                , _capacity
            );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringAssign( Char * const _buffer, size_t _capacity, const Char * _value, size_t _size )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = MENGINE_STRLEN( _value );
            }

            MENGINE_ASSERTION_FATAL( value_size < _capacity, "static string buffer assign '%s' [size %zu] > [capacity %zu]"
                , _value
                , value_size
                , _capacity
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            MENGINE_STRNCPY( _buffer, _value, value_size );
            _buffer[value_size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringAppendChar( Char * const _buffer, size_t _capacity, Char _value )
        {
            size_t buffer_size = MENGINE_STRLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size + 1 < _capacity, "static string buffer '%s' [size %zu] append char '%c' > [capacity %zu]"
            , _buffer
            , buffer_size
            , _value
            , _capacity
            );

            if( buffer_size + 1 >= _capacity )
            {
                return;
            }

            _buffer[buffer_size + 0] = _value;
            _buffer[buffer_size + 1] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringAppend( Char * const _buffer, size_t _capacity, const Char * _value, size_t _size )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = MENGINE_STRLEN( _value );
            }

            size_t buffer_size = MENGINE_STRLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size + value_size < _capacity, "static string buffer '%s' [size %zu] append '%s' [size %zu] > [capacity %zu]"
                , _buffer
                , buffer_size
                , _value
                , value_size
                , _capacity
            );

            if( buffer_size + value_size >= _capacity )
            {
                value_size = _capacity - buffer_size - 1;
            }

            MENGINE_STRNCAT( _buffer, _value, value_size );
            _buffer[buffer_size + value_size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringFormat( Char * const _buffer, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args )
        {
            MENGINE_ASSERTION_VSNPRINTF( _capacity, _format, _args );

            MENGINE_VSNPRINTF( _buffer, _capacity, _format, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringCopy( const Char * _buffer, size_t _capacity, Char * const _value )
        {
            MENGINE_STRNCPY( _value, _buffer, _capacity );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticStringCompare( const Char * _buffer, size_t _capacity, const Char * _value )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            size_t value_size = MENGINE_STRLEN( _value );

            if( value_size >= _capacity )
            {
                return 1;
            }

            int32_t result = MENGINE_STRNCMP( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}