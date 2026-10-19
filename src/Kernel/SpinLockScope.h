#pragma once

#include "Kernel/SpinLock.h"

namespace Mengine
{
    class SpinLockScope
    {
    public:
        SpinLockScope( SpinLock & _futex );
        ~SpinLockScope();

    private:
        SpinLockScope( const SpinLockScope & _scope );
        SpinLockScope & operator = ( const SpinLockScope & _scope );

    protected:
        SpinLock & m_spinlock;
    };
}

#define MENGINE_SPINLOCK_SCOPE_I( SpinLock, Index ) SpinLockScope MENGINE_PP_CONCATENATE(__spinlock_scope_, Index)( SpinLock )
#define MENGINE_SPINLOCK_SCOPE( SpinLock ) MENGINE_SPINLOCK_SCOPE_I( SpinLock, MENGINE_CODE_COUNTER )