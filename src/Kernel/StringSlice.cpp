#include "StringSlice.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        void stringSlice( const Char * _in, size_t _size, Char * _buffer, size_t _capacity, const LambdaStringSlice & _lambda )
        {
            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = MENGINE_STRLEN( _in );
            }

            size_t data_index = 0;

            for( ; data_index + _capacity < _size; data_index += _capacity )
            {
                MENGINE_MEMCPY( _buffer, _in + data_index, _capacity );
                _buffer[_capacity] = '\0';

                _lambda( _buffer );
            }

            if( data_index == _size )
            {
                return;
            }

            size_t tail_size = _size - data_index;

            MENGINE_MEMCPY( _buffer, _in + data_index, tail_size );
            _buffer[tail_size] = '\0';

            _lambda( _buffer );
        }
    }
}