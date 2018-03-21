#pragma once

#include "Interface/SocketInterface.h"

#include "Core/ServiceBase.h"

#include "Factory/Factory.h"

#include "Win32Socket.h"

#include <winsock2.h> 

namespace Mengine
{
	class Win32SocketSystem
		: public ServiceBase<SocketSystemInterface>
	{
	public:
		Win32SocketSystem();
		~Win32SocketSystem() override;

	public:
		bool _initialize() override;
		void _finalize() override;

	protected:
		SocketInterfacePtr createSocket() override;

	protected:
		WSADATA m_wsaData;

		FactoryPtr m_poolWin32Socket;
	};
}