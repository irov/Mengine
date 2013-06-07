#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "s3eThread.h"

namespace Menge
{
	class MarmaladeThreadIdentity
		: public ThreadIdentity
	{
	public:
		MarmaladeThreadIdentity( s3eThread * _thread );

	public:
		s3eThread * getThread() const;

	protected:
		s3eThread * m_thread;
	};
}
