#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
	class PosixThreadIdentity;
	class ServiceProviderInterface;
	class LogServiceInterface;

	class PosixThreadSystem
		: public ThreadSystemInterface
	{
	public:
		PosixThreadSystem();
		~PosixThreadSystem();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider ) override;
		void finalize() override;

	public:
		ThreadIdentity * createThread( ThreadListener * _listener ) override;
		void joinThread( ThreadIdentity * _thread ) override;
		void sleep( unsigned int _ms ) override;
	
	protected:
		typedef std::vector<PosixThreadIdentity *> TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;

		ServiceProviderInterface * m_serviceProvider;
		LogServiceInterface * m_logService;
	};
}
