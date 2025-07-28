#pragma once

#include "Config/Config.h"

#include "Kernel/Hashgen.h"

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T, class D = void>
    using IntrusivePtr = stdex::intrusive_ptr<T, D>;
    //////////////////////////////////////////////////////////////////////////
    template<class T, class D>
    struct Hashgen<IntrusivePtr<T, D>>
    {
        HashType operator()( const IntrusivePtr<T, D> & _value ) const MENGINE_NOEXCEPT
        {
            const typename IntrusivePtr<T, D>::value_type * ptr = _value.get();

            return (HashType)ptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T, class D = void>
        IntrusivePtr<T, D> makeIntrusivePtr( T * _ptr )
        {
            return IntrusivePtr<T, D>( _ptr );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
