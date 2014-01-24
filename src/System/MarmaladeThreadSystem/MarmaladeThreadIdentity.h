#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#   include "s3eThread.h"

namespace Menge
{
	class MarmaladeThreadIdentity
		: public ThreadIdentity
	{
	public:
		MarmaladeThreadIdentity();

    public:
        void initialize( s3eThread * _thread, const ThreadTaskInterfacePtr & _task );

	public:
		s3eThread * getThread() const;

	protected:
		s3eThread * m_thread;
		ThreadTaskInterfacePtr m_task;
	};

	typedef stdex::intrusive_ptr<MarmaladeThreadIdentity> MarmaladeThreadIdentityPtr;
}
