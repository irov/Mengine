#pragma once

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t make_crc32( const void * _buffer, size_t _size );
        uint32_t make_crc32_mod( const void * _buffer, size_t _size, uint32_t _mod );

        template<class T>
        uint32_t make_crc32_pod( const T & _t )
        {
            uint32_t value = make_crc32( &_t, sizeof( T ) );

            return value;
        }

        template<class T>
        uint32_t make_crc32_mod_pod( const T & _t, uint32_t _mod )
        {
            uint32_t value = make_crc32_mod( &_t, sizeof( T ), _mod );

            return value;
        }
    }
}