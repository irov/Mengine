#	pragma once
#	include "Config/Typedef.h"

namespace Menge
{
	class ThreadListener
	{
	public:
		virtual bool main() = 0;
		virtual void join() = 0;
	};

	class ThreadIdentity
	{
	};

	class ThreadSystemInterface
	{
	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ThreadIdentity * createThread( ThreadListener * _listener ) = 0;
		virtual void joinThread( ThreadIdentity * _thread ) = 0;
		virtual void sleep( unsigned int _ms ) = 0;
	};
}
bool	initInterfaceSystem( Menge::ThreadSystemInterface ** );
void	releaseInterfaceSystem( Menge::ThreadSystemInterface * );