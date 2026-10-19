#include "SpinLockTryScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SpinLockTryScope::SpinLockTryScope( SpinLock & _spinlock )
        : m_spinlock( _spinlock )
        , m_locked( _spinlock.tryLock() )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SpinLockTryScope::~SpinLockTryScope()
    {
        if( m_locked == false )
        {
            return;
        }

        m_spinlock.unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpinLockTryScope::isLocked() const
    {
        return m_locked;
    }
    //////////////////////////////////////////////////////////////////////////
}
