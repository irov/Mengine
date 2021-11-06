#include "Hexadecimal.h"

#include "Kernel/Assertion.h"

#include "Config/StdIO.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            static bool hexadecimalToUint8( Char _c, uint8_t * const _u8 )
            {
                switch( _c )
                {
                case '0': *_u8 = 0; break;
                case '1': *_u8 = 1; break;
                case '2': *_u8 = 2; break;
                case '3': *_u8 = 3; break;
                case '4': *_u8 = 4; break;
                case '5': *_u8 = 5; break;
                case '6': *_u8 = 6; break;
                case '7': *_u8 = 7; break;
                case '8': *_u8 = 8; break;
                case '9': *_u8 = 9; break;
                case 'a': *_u8 = 10; break;
                case 'b': *_u8 = 11; break;
                case 'c': *_u8 = 12; break;
                case 'd': *_u8 = 13; break;
                case 'e': *_u8 = 14; break;
                case 'f': *_u8 = 15; break;
                default:
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool encodeHexadecimal( const void * _data, size_t _datasize, Char * const _hexadecimal, size_t _capacity, size_t * const _outsize )
        {
            if( _capacity < _datasize * 2 )
            {
                return false;
            }

            Char * c = _hexadecimal;
            const uint8_t * d = reinterpret_cast<const uint8_t *>(_data);

            for( size_t i = 0; i != _datasize; ++i )
            {
                MENGINE_SPRINTF( c, "%02x", d[i] );

                c += 2;
            }

            *(c - 1) = '\0';

            if( _outsize != nullptr )
            {
                *_outsize = _datasize * 2;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool decodeHexadecimal( const Char * _hexadecimal, size_t _size, void * const _data, size_t _capacity, size_t * const _outsize )
        {
            if( _size % 2 != 0 )
            {
                return false;
            }

            if( _capacity < _size / 2 )
            {
                return false;
            }

            uint8_t * d = reinterpret_cast<uint8_t *>(_data);

            for( size_t i = 0; i != _size; i += 2 )
            {
                Char c0 = _hexadecimal[i + 0];
                Char c1 = _hexadecimal[i + 1];

                uint8_t u0;
                if( Detail::hexadecimalToUint8( c0, &u0 ) == false )
                {
                    return false;
                }

                uint8_t u1;
                if( Detail::hexadecimalToUint8( c1, &u1 ) == false )
                {
                    return false;
                }

                *(d++) = u0 + (u1 << 8);
            }

            if( _outsize != nullptr )
            {
                *_outsize = _size / 2;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}