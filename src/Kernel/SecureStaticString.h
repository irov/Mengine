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
        constexpr const uint32_t * secureStaticString( uint32_t _token, Args ... _args )
        {
            static uint32_t buff[sizeof...(Args) + 2] = {sizeof...(Args), _token,
                (((uint32_t)_args) ^ (_token = ((_token * MENGINE_SECURE_DATA_TOKEN_BASE) + MENGINE_SECURE_DATA_TOKEN_STROKE))) ...
            };

            return buff;
        }

        void unsecureStaticString( const uint32_t * _data, Char * const _str );
    }
}