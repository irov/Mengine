#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    class Factorable;

    namespace Helper
    {
        const ConstString & getFactorableType( const Factorable * _factorable );

        template<class T, class D>
        const ConstString & getFactorableType( const IntrusivePtr<T, D> & _factorable )
        {
            return Helper::getFactorableType( _factorable.get() );
        }
    }
}
