#include "StaticWString.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        size_t staticWStringSize( const WChar * _buffer, size_t _capacity )
        {
            MENGINE_UNUSED( _capacity );

            size_t buffer_size = StdString::wcslen( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size < _capacity, "static wstring buffer '%ls' [size %zu] > [capacity %zu]"
                , _buffer
                , buffer_size
                , _capacity
            );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        WChar staticWStringIndex( const WChar * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity, "static wstring buffer '%ls' [index %zu] > [capacity %zu]"
                , _buffer
                , _index
                , _capacity
            );

            WChar c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringChange( WChar * _buffer, size_t _capacity, size_t _index, WChar _ch )
        {
            MENGINE_UNUSED( _capacity );

            MENGINE_ASSERTION_FATAL( _index < _capacity, "static wstring buffer '%ls' [index %zu] > [capacity %zu]"
                , _buffer
                , _index
                , _capacity
            );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringAssign( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = StdString::wcslen( _value );
            }

            MENGINE_ASSERTION_FATAL( value_size < _capacity, "static wstring buffer assign '%ls' [size %zu]"
                , _value
                , value_size
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            StdString::wcsncpy( _buffer, _value, value_size );
            _buffer[value_size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringAppend( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            size_t value_size = _size;

            if( value_size == MENGINE_UNKNOWN_SIZE )
            {
                value_size = StdString::wcslen( _value );
            }

            size_t buffer_size = StdString::wcslen( _buffer );

            MENGINE_ASSERTION_FATAL( buffer_size + value_size < _capacity, "static string buffer '%ls' [size %zu] append '%ls' [size %zu] > [capacity %zu]"
                , _buffer
                , buffer_size
                , _value
                , value_size
                , _capacity
            );

            if( value_size >= _capacity )
            {
                value_size = _capacity - 1;
            }

            StdString::wcsncat( _buffer, _value, value_size );
            _buffer[buffer_size + value_size] = L'\0';
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringCopy( const WChar * _buffer, size_t _capacity, WChar * _value )
        {
            MENGINE_UNUSED( _capacity );

            StdString::wcscpy( _value, _buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticWStringCompare( const WChar * _buffer, size_t _capacity, const WChar * _value )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value );

            int32_t result = StdString::wcsncmp( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}