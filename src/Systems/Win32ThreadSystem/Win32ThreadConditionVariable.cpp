#include "Win32ThreadConditionVariable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadConditionVariable::Win32ThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadConditionVariable::~Win32ThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32ThreadConditionVariable::initialize()
    {
#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        ::InitializeCriticalSection( &m_conditionLock );
        ::InitializeConditionVariable( &m_conditionVariable );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::finalize()
    {
#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        ::DeleteCriticalSection( &m_conditionLock );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::wait( const LambdaCondition & _condition )
    {
        for( ;; )
        {
#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
            ::EnterCriticalSection( &m_conditionLock );
            ::SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, 1000 );
            ::LeaveCriticalSection( &m_conditionLock );
#endif

            if( _condition() == true )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::wake()
    {
#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        ::WakeConditionVariable( &m_conditionVariable );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::_destroy()
    {
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
}