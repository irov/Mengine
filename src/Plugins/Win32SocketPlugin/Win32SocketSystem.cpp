#include "Win32SocketSystem.h"

#include "Win32Socket.h"

#include "Kernel/Logger.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"

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
            LOGGER_ERROR( "invalid WSAStartup [%d]"
                , WSAStartup_result
            );

            return false;
        }

        m_factoryWin32Socket = Helper::makeFactoryPool<Win32Socket, 16>( MENGINE_DOCUMENT_FACTORABLE );

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
    SocketInterfacePtr Win32SocketSystem::createSocket( const DocumentInterfacePtr & _doc )
    {
        Win32SocketPtr socket = m_factoryWin32Socket->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( socket );

        return socket;
    }
    //////////////////////////////////////////////////////////////////////////
}