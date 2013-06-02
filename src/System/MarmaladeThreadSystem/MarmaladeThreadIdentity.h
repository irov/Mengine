#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include <pthread.h>
namespace Menge
{
	class PosixThreadIdentity
		: public ThreadIdentity
	{
	public:
		PosixThreadIdentity( pthread_t _id );

	public:
		pthread_t getId() const;

	protected:
		pthread_t m_id;
	};
}
