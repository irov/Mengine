#pragma once

#include "Config/Char.h"

#include <type_traits>

namespace Mengine
{
    struct ThreadDescription
    {
        const Char * nameA = "/0";
        const WChar * nameW = L"/0";
    };

    namespace Detail
    {
        template<size_t N, typename = std::enable_if_t<(N <= MENGINE_MAX_THREAD_NAME + 1)>>
        ThreadDescription validateThreadDescription( const Char( &nameA )[N], const WChar( &nameW )[N] )
        {
            return ThreadDescription{nameA, nameW};
        }
    }
}

#define MENGINE_THREAD_DESCRIPTION( Name ) Mengine::Detail::validateThreadDescription( Name, MENGINE_PP_CONCATENATE(L, Name) )