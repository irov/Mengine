#include "StaticWString.h"

#include "Config/StdAssert.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        size_t staticWStringSize( const WChar * _buffer, size_t _capacity )
        {
            MENGINE_UNUSED( _capacity );

            size_t buffer_size = StdString::wcslen( _buffer );

            assert( buffer_size < _capacity );

            return buffer_size;
        }
        //////////////////////////////////////////////////////////////////////////
        WChar staticWStringIndex( const WChar * _buffer, size_t _capacity, size_t _index )
        {
            MENGINE_UNUSED( _capacity );

            assert( _index < _capacity );

            WChar c = _buffer[_index];

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        void staticWStringChange( WChar * _buffer, size_t _capacity, size_t _index, WChar _ch )
        {
            MENGINE_UNUSED( _capacity );

            assert( _index < _capacity );
            assert( _index < _capacity - 1 || _ch == L'\0' );

            _buffer[_index] = _ch;
        }
        //////////////////////////////////////////////////////////////////////////
        int32_t staticWStringCompare( const WChar * _buffer, size_t _capacity, const WChar * _value )
        {
            assert( _value != nullptr );

            int32_t result = StdString::wcsncmp( _buffer, _value, _capacity );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}