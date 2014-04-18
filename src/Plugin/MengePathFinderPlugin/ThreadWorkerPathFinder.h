#	pragma once

#	include "Kernel/ThreadJob.h"

namespace Menge
{
	class ThreadWorkerPathFinder
		: public ThreadWorkerInterface
	{
	public:
		ThreadWorkerPathFinder();
		~ThreadWorkerPathFinder();

	public:        
		void initialize( ServiceProviderInterface * _serviceProvider );

	public:
		bool isDone() const;

	public:
		bool onWork( size_t _id ) override;
		void onDone( size_t _id ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		bool m_done;
	};

	typedef stdex::intrusive_ptr<ThreadWorkerPathFinder> ThreadWorkerPathFinderPtr;
}	// namespace Menge
