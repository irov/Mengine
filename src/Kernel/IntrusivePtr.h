#pragma once

#include "Config/Config.h"

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T, class D = void>
    using IntrusivePtr = stdex::intrusive_ptr<T, D>;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T, class D = void>
        IntrusivePtr<T, D> makeIntrusivePtr( T * _ptr )
        {
            return IntrusivePtr<T, D>( _ptr );
        }
    }
}
