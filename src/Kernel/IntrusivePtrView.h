#pragma once

#include "Config/Config.h"

#include "stdex/intrusive_ptr_view.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T, class D = void>
    using IntrusivePtrView = stdex::intrusive_ptr_view<T, D>;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T, class D = void>
        IntrusivePtrView<T, D> makeIntrusivePtrView( const T * _ptr )
        {
            return IntrusivePtrView<T, D>( _ptr );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
