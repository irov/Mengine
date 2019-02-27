#pragma once

#include "Kernel/Factory.h"

namespace Mengine
{
    namespace Detail
    {
        void assertionFactoryEmpty( const FactoryPtr & _factory, const Char * _file, uint32_t _line );
    }
}

#ifdef MENGINE_DEBUG
#   define MENGINE_ASSERTION_FACTORY_EMPTY(F) Detail::assertionFactoryEmpty(F, __FILE__, __LINE__)
#else
#   define MENGINE_ASSERTION_FACTORY_EMPTY(F)
#endif

