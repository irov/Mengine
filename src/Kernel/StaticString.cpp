#include "StaticString.h"

#include "Config/StdAssert.h"
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

            assert( buffer_size < _capacity );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        Char staticStringIndex( const Char * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            assert( _index < _capacity );

            Char c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringChange( Char * const _buffer, size_t _capacity, size_t _index, Char _ch )
        {
            MENGINE_UNUSED( _capacity );

            assert( _index < _capacity );
            assert( _index < _capacity - 1 || _ch == '\0' );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticStringFormat( Char * const _buffer, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args )
        {
            int32_t result = StdIO::vsnprintf( _buffer, _capacity, _format, _args );

            MENGINE_UNUSED( result );

            assert( result >= 0 );
            assert( (size_t)result < _capacity );
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticStringCompare( const Char * _buffer, size_t _capacity, const Char * _value )
        {
            assert( _value != nullptr );

            int32_t result = StdString::strncmp( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}