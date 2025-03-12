#pragma once

#include "Kernel/Futex.h"

namespace Mengine
{
    class FutexScope
    {
    public:
        FutexScope( Futex & _futex );
        ~FutexScope();

    protected:
        Futex & m_futex;
    };
}

#define MENGINE_FUTEX_SCOPE_I( Futex, Line ) FutexScope __futex_scope_##Line( Futex )
#define MENGINE_FUTEX_SCOPE( Futex ) MENGINE_FUTEX_SCOPE_I( Futex, MENGINE_CODE_LINE )