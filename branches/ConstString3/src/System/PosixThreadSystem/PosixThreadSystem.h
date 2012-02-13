#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
	class PosixThreadIdentity;

	class PosixThreadSystem
		: public ThreadSystemInterface
	{
	public:
		PosixThreadSystem();
		virtual ~PosixThreadSystem();
	public:
		bool initialize() override;
		void finalize() override;

	public:
		ThreadIdentity * createThread( ThreadListener * _listener ) override;
		void joinThread( ThreadIdentity * _thread ) override;
		void sleep( unsigned int _ms ) override;
	
	protected:
		typedef std::vector< PosixThreadIdentity * > TVectorPosixThreadIdentity;
		TVectorPosixThreadIdentity m_threadIdentities;
	};
}
