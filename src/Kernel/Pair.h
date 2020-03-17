#pragma once

#include <utility>

namespace Mengine
{
    template<class V1, class V2>
    using Pair = std::pair<V1, V2>;

    namespace Helper
    {
        template<class V1, class V2>
        Pair<V1, V2> makePair( V1 && _v1, V2 && _v2 )
        {
            return Pair<V1, V2>( _v1, _v2 );
        }
    }
}
