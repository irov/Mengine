#include "Win32SocketSystem.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Win32Socket.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Mengine::Win32SocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32SocketSystem::Win32SocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32SocketSystem::~Win32SocketSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32SocketSystem::_initializeService()
    {
        WORD wVersionRequested = MAKEWORD( 2, 2 );

        int32_t WSAStartup_result = ::WSAStartup( wVersionRequested, &m_wsaData );

        if( WSAStartup_result != 0 )
        {
            return false;
        }

        m_factoryWin32Socket = new FactoryPool<Win32Socket, 16>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32SocketSystem::_finalizeService()
    {
        ::WSACleanup();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryWin32Socket );

        m_factoryWin32Socket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketInterfacePtr Win32SocketSystem::createSocket( const Char * _doc )
    {
        Win32SocketPtr socket = m_factoryWin32Socket->createObject( _doc );

        return socket;
    }
}