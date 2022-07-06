#include "ThreadGuardScope.h"

#include "Interface/ThreadSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Crash.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadGuardScope::ThreadGuardScope( const ThreadGuard & _guard, const Char * _doc )
        : m_guard( _guard )
        , m_doc( _doc )
    {
        if( m_guard.lock( true ) == true )
        {
            uint64_t lockThreadId = m_guard.getLockThreadId();

            const ConstString & lockThreadName = THREAD_SERVICE()
                ->findThreadNameById( lockThreadId );

            uint64_t currentThreadId = THREAD_SYSTEM()
                ->getCurrentThreadId();

            const ConstString & currentThreadName = THREAD_SERVICE()
                ->findThreadNameById( currentThreadId );

            LOGGER_ERROR( "thread safe issue: [%s] current thread '%s' lock thread '%s'"
                , _doc
                , currentThreadName.c_str()
                , lockThreadName.c_str()
            );

            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuardScope::~ThreadGuardScope()
    {
        if( m_guard.lock( false ) == false )
        {
            uint64_t lockThreadId = m_guard.getLockThreadId();

            const ConstString & lockThreadName = THREAD_SERVICE()
                ->findThreadNameById( lockThreadId );

            uint64_t currentThreadId = THREAD_SYSTEM()
                ->getCurrentThreadId();

            const ConstString & currentThreadName = THREAD_SERVICE()
                ->findThreadNameById( currentThreadId );

            LOGGER_ERROR( "thread safe issue: [%s] current thread '%s' lock thread '%s'"
                , m_doc
                , currentThreadName.c_str()
                , lockThreadName.c_str()
            );

            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}