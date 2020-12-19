#include "ThreadGuardScope.h"

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
            LOGGER_ERROR( "thread safe issue: %s"
                , _doc
            );

            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuardScope::~ThreadGuardScope()
    {
        if( m_guard.lock( false ) == false )
        {
            LOGGER_ERROR( "thread safe issue: %s"
                , m_doc
            );

            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}