#pragma once

#include "Kernel/SpinLock.h"

namespace Mengine
{
    class SpinLockTryScope
    {
    public:
        explicit SpinLockTryScope( SpinLock & _spinlock );
        ~SpinLockTryScope();

    public:
        bool isLocked() const;

    private:
        SpinLockTryScope( const SpinLockTryScope & _scope );
        SpinLockTryScope & operator = ( const SpinLockTryScope & _scope );

    protected:
        SpinLock & m_spinlock;
        bool m_locked;
    };
}
