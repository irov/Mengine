#include "StaticString.h"

#include "Kernel/Assertion.h"

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

            MENGINE_ASSERTION_FATAL( buffer_size < _capacity );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        Char staticStringIndex( const Char * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity );

            Char c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringChange( Char * _buffer, size_t _capacity, size_t _index, Char _ch )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringAssign( Char * _buffer, size_t _capacity, const Char * _value, size_t _size )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            size_t value_size = _size;

            if( value_size == ~0U )
            {
                value_size = MENGINE_STRLEN( _value );
            }

            MENGINE_ASSERTION_FATAL( value_size < _capacity, "static string buffer assign '%s' [size %zu]"
                , _value
                , value_size
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            MENGINE_STRNCPY( _buffer, _value, value_size );
            _buffer[value_size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringAppend( Char * _buffer, size_t _capacity, const Char * _value, size_t _size )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            size_t value_size = _size;

            if( value_size == ~0U )
            {
                value_size = MENGINE_STRLEN( _value );
            }

            size_t buffer_size = MENGINE_STRLEN( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size + value_size < _capacity, "static string buffer '%s' [size %zu] append '%s' [size %zu]"
                , _buffer
                , buffer_size
                , _value
                , value_size
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            MENGINE_STRNCAT( _buffer, _value, value_size );
            _buffer[buffer_size + value_size] = '\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringFormat( Char * _buffer, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args )
        {
            MENGINE_ASSERTION_FATAL( (size_t)MENGINE_VSNPRINTF( nullptr, 0, _format, _args ) < _capacity );

            MENGINE_VSNPRINTF( _buffer, _capacity, _format, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringCopy( const Char * _buffer, size_t _capacity, Char * _value )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_STRCPY( _value, _buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticStringCompare( const Char * _buffer, size_t _capacity, const Char * _value )
        {
            MENGINE_ASSERTION_FATAL( _value != nullptr );

            int32_t result = MENGINE_STRNCMP( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}