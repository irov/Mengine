#include "StringSlice.h"

#include "Kernel/Utf8Helper.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool stringSlice( const Char * _in, size_t _size, Char * _buffer, size_t _capacity, const LambdaStringSlice & _lambda )
        {
            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::strlen( _in );
            }

            size_t data_index = 0;

            for( ; data_index + _capacity < _size; data_index += _capacity )
            {
                StdString::memcpy( _buffer, _in + data_index, _capacity );
                _buffer[_capacity] = '\0';

                if( _lambda( _buffer ) == false )
                {
                    return false;
                }
            }

            if( data_index == _size )
            {
                return true;
            }

            size_t tail_size = _size - data_index;

            StdString::memcpy( _buffer, _in + data_index, tail_size );
            _buffer[tail_size] = '\0';

            if( _lambda( _buffer ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool utf8Slice( const Utf8 * _in, size_t _size, Utf8 * _buffer, size_t _capacity, const LambdaStringUtf8Slice & _lambda )
        {
            if( _size == MENGINE_UNKNOWN_SIZE )
            {
                _size = StdString::strlen( _in );
            }

            const Utf8 * it_slice = _in;
            const Utf8 * it_correct = _in;
            const Utf8 * it_current = _in;
            const Utf8 * it_end = _in + _size;

            while( it_current != it_end )
            {
                if( Helper::Utf8NextCode( &it_current, it_end, nullptr ) == false )
                {
                    return false;
                }

                size_t carriage_size = it_current - it_slice;

                if( carriage_size > _capacity )
                {
                    it_current = it_correct;

                    size_t correct_size = it_correct - it_slice;

                    StdString::memcpy( _buffer, it_slice, correct_size );
                    _buffer[correct_size] = '\0';

                    if( _lambda( _buffer ) == false )
                    {
                        return false;
                    }

                    it_slice = it_correct;
                }
                else
                {
                    it_correct = it_current;
                }
            }

            if( it_slice == it_end )
            {
                return true;
            }

            size_t tail_size = it_end - it_slice;

            StdString::memcpy( _buffer, it_slice, tail_size );
            _buffer[tail_size] = '\0';

            if( _lambda( _buffer ) == false )
            {
                return false;
            }

            return true;
        }
        ///////////////////////////////////////////////////////////////////////
    }
}