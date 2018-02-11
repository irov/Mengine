#	include "Win32SocketSystem.h"

#   include "Factory/FactoryPool.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketSystem, Menge::Win32SocketSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
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
	bool Win32SocketSystem::_initialize()
	{
        WORD wVersionRequested = MAKEWORD( 2, 2 );

		int WSAStartup_result = WSAStartup( wVersionRequested, &m_wsaData );

		if( WSAStartup_result != 0 )
		{
			return false;
		}

		m_poolWin32Socket = new FactoryPool<Win32Socket, 16>();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32SocketSystem::_finalize()
	{
		WSACleanup();
	}
	//////////////////////////////////////////////////////////////////////////
	SocketInterfacePtr Win32SocketSystem::createSocket()
	{
		SocketInterfacePtr socket = m_poolWin32Socket->createObject();

		return socket;
	}
}