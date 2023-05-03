#pragma once

#include "Config/Config.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/FactoryInterface.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        void assertionFactoryEmpty( const Char * _category, const FactoryInterfacePtr & _factory, const Char * _file, uint32_t _line );
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_FACTORY_EMPTY(F) Detail::assertionFactoryEmpty(MENGINE_CODE_LIBRARY, F, MENGINE_CODE_FILE, MENGINE_CODE_LINE)
#else
#   define MENGINE_ASSERTION_FACTORY_EMPTY(F)
#endif
