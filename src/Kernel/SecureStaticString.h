#pragma once

#include "Config/Config.h"
#include "Config/StdInt.h"
#include "Config/Char.h"

#ifndef MENGINE_SECURE_DATA_TOKEN_BASE
#define MENGINE_SECURE_DATA_TOKEN_BASE 1103515245
#endif

#ifndef MENGINE_SECURE_DATA_TOKEN_STROKE
#define MENGINE_SECURE_DATA_TOKEN_STROKE 12345
#endif

namespace Mengine
{
    namespace Helper
    {
        template<class ... Args>
        constexpr void secureStaticString( uint32_t * const _data, uint32_t _token, Args ... _args )
        {
            _data[0] = sizeof...(Args);
            _data[1] = _token;

            uint32_t secure_buff[sizeof...(Args)] = {(((uint32_t)_args) ^ (_token = (_token * MENGINE_SECURE_DATA_TOKEN_BASE) + MENGINE_SECURE_DATA_TOKEN_STROKE)) ...};

            for( uint32_t index = 0; index != sizeof...(Args); ++index )
            {
                _data[index + 2] = secure_buff[index];
            }
        }

        void unsecureStaticString( const uint32_t * _data, Char * const _str );
    }
}
