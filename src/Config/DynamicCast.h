#pragma once

#include "Config/Config.h"

namespace Mengine
{
    namespace Helper
    {
#if MENGINE_RTTI == 1
        template<class T, class U>
        T dynamicCast( U _ptr )
        {
            return dynamic_cast<T>(_ptr);
        }
#else
        template<class T, class U>
        T dynamicCast( U _ptr )
        {
            MENGINE_UNUSED( _ptr );

            return nullptr;
        }
#endif
    }
}