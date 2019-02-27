#include "Win32ThreadConditionVariable.h"

#include "Config/Config.h"

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
    void Win32ThreadConditionVariable::initialize( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

#ifdef MENGINE_DEBUG
        m_doc = _doc;
#endif

        InitializeCriticalSection( &m_conditionLock );
        InitializeConditionVariable( &m_conditionVariable );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::wait()
    {
        EnterCriticalSection( &m_conditionLock );
        SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, INFINITE );
        LeaveCriticalSection( &m_conditionLock );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::wake()
    {
        WakeConditionVariable( &m_conditionVariable );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::_destroy()
    {
        WakeAllConditionVariable( &m_conditionVariable );
        DeleteCriticalSection( &m_conditionLock );
    }
}