#pragma once

#include "stdex/exception.h"

#define MENGINE_THROW_EXCEPTION STDEX_THROW_EXCEPTION
#define MENGINE_THROW_EXCEPTION_FL stdex::helper::throw_exception

namespace Mengine
{
    typedef stdex::helper::stdex_exception Exception;
}


