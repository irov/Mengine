#include "Base64.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_CONSTEXPR Char base64_encode_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'};
        //////////////////////////////////////////////////////////////////////////
        static MENGINE_CONSTEXPR uint8_t base64_decode_table[256] = {
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
        size_t getBase64EncodeSize( size_t _size )
        {
            size_t totalsize = 4 * ((_size + 2) / 3);

            return totalsize;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t getBase64DecodeSize( const Char * _base64, size_t _size )
        {
            if( _size == 0 )
            {
                return 0;
            }

            if( _size % 4 != 0 )
            {
                return 0;
            }

            size_t padding = 0;

            if( _base64[_size - 1] == '=' )
            {
                ++padding;

                if( _base64[_size - 2] == '=' )
                {
                    ++padding;
                }
            }

            size_t symbolCount = _size - padding;

            for( size_t index = 0; index != symbolCount; ++index )
            {
                uint8_t symbol = static_cast<uint8_t>(_base64[index]);

                if( base64_decode_table[symbol] == 0 && symbol != 'A' )
                {
                    return 0;
                }
            }

            size_t totalsize = _size / 4 * 3 - padding;

            return totalsize;
        }
        //////////////////////////////////////////////////////////////////////////
        void encodeBase64( const void * _data, size_t _datasize, Char * const _base64 )
        {
            size_t totalsize = Helper::getBase64EncodeSize( _datasize );

            const uint8_t * data_u8 = static_cast<const uint8_t *>(_data);

            for( size_t i = 0, j = 0; i != _datasize;)
            {
                uint32_t octet_a = i < _datasize ? data_u8[i++] : 0;
                uint32_t octet_b = i < _datasize ? data_u8[i++] : 0;
                uint32_t octet_c = i < _datasize ? data_u8[i++] : 0;

                uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

                _base64[j++] = base64_encode_table[(triple >> 3 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 2 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 1 * 6) & 0x3F];
                _base64[j++] = base64_encode_table[(triple >> 0 * 6) & 0x3F];
            }

            const uint32_t mod_table[] = {0, 2, 1};

            const uint32_t mod = mod_table[_datasize % 3];

            for( uint32_t i = 0; i != mod; ++i )
            {
                _base64[totalsize - 1 - i] = '=';
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void encodeBase64( const Data & _data, String * const _base64 )
        {
            const void * dataBuffer = _data.data();
            size_t dataSize = _data.size();
            size_t base64Size = Helper::getBase64EncodeSize( dataSize );

            _base64->resize( base64Size );

            Char * base64Buffer = _base64->data();

            Helper::encodeBase64( dataBuffer, dataSize, base64Buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        bool decodeBase64( const Char * _base64, size_t _base64size, void * const _data )
        {
            if( _base64size == 0 )
            {
                return true;
            }

            size_t totalsize = Helper::getBase64DecodeSize( _base64, _base64size );

            if( totalsize == 0 )
            {
                return false;
            }

            uint8_t * const data_u8 = static_cast<uint8_t * const>(_data);

            for( size_t i = 0, j = 0; i != _base64size; i += 4 )
            {
                uint32_t sextet_a = _base64[i + 0] == '=' ? 0 : base64_decode_table[(uint8_t)_base64[i + 0]];
                uint32_t sextet_b = _base64[i + 1] == '=' ? 0 : base64_decode_table[(uint8_t)_base64[i + 1]];
                uint32_t sextet_c = _base64[i + 2] == '=' ? 0 : base64_decode_table[(uint8_t)_base64[i + 2]];
                uint32_t sextet_d = _base64[i + 3] == '=' ? 0 : base64_decode_table[(uint8_t)_base64[i + 3]];

                uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

                if( j < totalsize ) data_u8[j++] = (triple >> 2 * 8) & 0xFF;
                if( j < totalsize ) data_u8[j++] = (triple >> 1 * 8) & 0xFF;
                if( j < totalsize ) data_u8[j++] = (triple >> 0 * 8) & 0xFF;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool decodeBase64( const String & _base64, Data * const _data )
        {
            const Char * base64Buffer = _base64.c_str();
            size_t base64Size = _base64.size();
            size_t dataSize = Helper::getBase64DecodeSize( base64Buffer, base64Size );

            _data->resize( dataSize );

            void * dataBuffer = _data->data();

            if( Helper::decodeBase64( base64Buffer, base64Size, dataBuffer ) == false )
            {
                _data->clear();

                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
