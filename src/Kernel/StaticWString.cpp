#include "StaticWString.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

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
        int32_t staticWStringCompare( const WChar * _buffer, size_t _size, const WChar * _value )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value, "invalid compare value" );

            int32_t result = StdString::wcsncmp( _buffer, _value, _size );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}