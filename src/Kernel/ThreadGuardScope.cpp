#include "ThreadGuardScope.h"

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
            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadGuardScope::~ThreadGuardScope()
    {
        if( m_guard.lock( false ) == false )
        {
            Helper::crash( m_doc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}