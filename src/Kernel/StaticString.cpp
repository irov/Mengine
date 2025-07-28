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

            size_t buffer_size = StdString::strlen( _buffer );

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
        void staticStringFormat( Char * const _buffer, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args )
        {
            MENGINE_ASSERTION_VSNPRINTF( _capacity, _format, _args );

            MENGINE_VSNPRINTF( _buffer, _capacity, _format, _args );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticStringCompare( const Char * _buffer, size_t _size, const Char * _value )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _value, "invalid compare value" );

            int32_t result = StdString::strncmp( _buffer, _value, _size );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}