#pragma once

#include "Config/Char.h"

#include "Config/StdDef.h"

namespace Mengine
{
    struct ThreadDescription
    {
        const Char * nameA = "/0";
        const WChar * nameW = L"/0";
    };

    namespace Detail
    {
        template<size_t N>
            requires (N <= MENGINE_MAX_THREAD_NAME + 1)
        ThreadDescription validateThreadDescription( const Char( &_nameA )[N], const WChar( &_nameW )[N] )
        {
            ThreadDescription desc{_nameA, _nameW};

            return desc;
        }
    }
}

#define MENGINE_THREAD_DESCRIPTION( Name ) Mengine::Detail::validateThreadDescription( Name, MENGINE_PP_CONCATENATE(L, Name) )