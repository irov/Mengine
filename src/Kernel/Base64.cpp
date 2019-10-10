#include "Base64.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char base64_encode_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'};
        //////////////////////////////////////////////////////////////////////////
        const uint8_t base64_decode_table[256] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //15
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //31
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, //47
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, //63
            0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, //79
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, //95
            0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //111
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, //127
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //143
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //159
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //175
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //191
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //207
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //223
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //239
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //255
        };
        //////////////////////////////////////////////////////////////////////////
        size_t base64_encode_size( size_t _size )
        {
            size_t totalsize = 4 * ((_size + 2) / 3);

            return totalsize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t base64_decode_size( const Char * _base64, size_t _size )
        {
            size_t totalsize = _size / 4 * 3;

            if( _base64[_size - 1] == '=' )
            {
                --totalsize;
            }

            if( _base64[_size - 2] == '=' )
            {
                --totalsize;
            }

            return totalsize;
        }
        //////////////////////////////////////////////////////////////////////////
        bool base64_encode( const uint8_t * _data, size_t _datasize, bool _mod, Char * _base64, size_t _capacity, size_t * _outsize )
        {
            size_t totalsize = base64_encode_size( _datasize );

            if( totalsize > _capacity )
            {
                return false;
            }

            for( uint32_t i = 0, j = 0; i != _datasize;)
            {
                uint32_t octet_a = i < _datasize ? _data[i++] : 0;
                uint32_t octet_b = i < _datasize ? _data[i++] : 0;
                uint32_t octet_c = i < _datasize ? _data[i++] : 0;

                uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

                _base64[j++] = base64_encode_table[(triple >> 3 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 2 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 1 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 0 * 6) & 0x3F];
            }

            const uint32_t mod_table[] = {0, 2, 1};

            const uint32_t mod = mod_table[_datasize % 3];

            if( _mod == true )
            {
                for( uint32_t i = 0; i != mod; ++i )
                {
                    _base64[totalsize - 1 - i] = '=';
                }
            }
            else
            {
                totalsize -= mod;
            }

            *_outsize = totalsize;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool base64_decode( const Char * _base64, size_t _base64size, uint8_t * _data, size_t _capacity, size_t * _outsize )
        {               
            size_t totalsize = base64_decode_size( _base64, _base64size );

            if( totalsize > _capacity )
            {
                return false;
            }
            
            for( uint32_t i = 0, j = 0; i != _base64size;)
            {
                uint32_t sextet_a = _base64[i] == '=' ? 0 & i++ : base64_decode_table[_base64[i++]];
                uint32_t sextet_b = _base64[i] == '=' ? 0 & i++ : base64_decode_table[_base64[i++]];
                uint32_t sextet_c = _base64[i] == '=' ? 0 & i++ : base64_decode_table[_base64[i++]];
                uint32_t sextet_d = _base64[i] == '=' ? 0 & i++ : base64_decode_table[_base64[i++]];

                uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

                if( j < totalsize ) _data[j++] = (triple >> 2 * 8) & 0xFF;
                if( j < totalsize ) _data[j++] = (triple >> 1 * 8) & 0xFF;
                if( j < totalsize ) _data[j++] = (triple >> 0 * 8) & 0xFF;
            }

            *_outsize = totalsize;

            return true;
        }
    }
}