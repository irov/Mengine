#	pragma once

#	include "Interface/SocketInterface.h"

#   include "Factory/FactoryStore.h"

#	include "Win32Socket.h"

#	include <winsock2.h> 

namespace Menge
{
	class Win32SocketSystem
		: public ServiceBase<SocketSystemInterface>
	{
	public:
		Win32SocketSystem();
		~Win32SocketSystem();

	public:
		bool _initialize() override;
		void _finalize() override;

	protected:
		SocketInterfacePtr createSocket() override;

	protected:
		WSADATA m_wsaData;

		typedef FactoryPoolStore<Win32Socket, 16> TPoolWin32Socket;
		TPoolWin32Socket m_poolWin32Socket;
	};
}