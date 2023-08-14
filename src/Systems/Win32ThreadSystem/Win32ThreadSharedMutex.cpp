#include "Win32ThreadSharedMutex.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadSharedMutex::Win32ThreadSharedMutex()
        : m_srwLock( SRWLOCK_INIT )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadSharedMutex::~Win32ThreadSharedMutex()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadSharedMutex::initialize()
    {
        ::InitializeSRWLock( &m_srwLock );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSharedMutex::finalize()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSharedMutex::lock()
    {
        ::AcquireSRWLockExclusive( &m_srwLock );
    }
    //////////////////////////////////////////////////////////////////////////
    _Requires_exclusive_lock_held_( m_srwLock ) void Win32ThreadSharedMutex::unlock()
    {  
        ::ReleaseSRWLockExclusive( &m_srwLock );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadSharedMutex::tryLock()
    {
        if( ::TryAcquireSRWLockExclusive( &m_srwLock ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadSharedMutex::lockShared()
    {
        ::AcquireSRWLockShared( &m_srwLock );
    }
    //////////////////////////////////////////////////////////////////////////
    _Requires_lock_held_( m_srwLock ) void Win32ThreadSharedMutex::unlockShared()
    {
        ::ReleaseSRWLockShared( &m_srwLock );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadSharedMutex::tryLockShared()
    {
        if( ::TryAcquireSRWLockShared( &m_srwLock ) == FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}