#include "Win32ThreadConditionVariable.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadConditionVariable::Win32ThreadConditionVariable()
#ifndef NDEBUG
        : m_file( nullptr )
        , m_line( 0 )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32ThreadConditionVariable::~Win32ThreadConditionVariable()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::initialize( const Char * _file, uint32_t _line )
    {
        MENGINE_UNUSED( _file );
        MENGINE_UNUSED( _line );

#ifndef NDEBUG
        m_file = _file;
        m_line = _line;
#endif

        InitializeCriticalSection( &m_conditionLock );
        InitializeConditionVariable( &m_conditionVariable );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32ThreadConditionVariable::wait()
    {
        SleepConditionVariableCS( &m_conditionVariable, &m_conditionLock, INFINITE );
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