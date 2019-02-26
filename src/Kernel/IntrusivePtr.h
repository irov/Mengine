#pragma once

#include "Config/Config.h"

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    template<class T, class D = void>
    using IntrusivePtr = stdex::intrusive_ptr<T, D>;

    namespace Helper
    {
        template<class T>
        IntrusivePtr<T> makeIntrusivePtr( T * _ptr )
        {
            return IntrusivePtr<T>( _ptr );
        }
    }
}
