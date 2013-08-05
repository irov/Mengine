#	pragma once

#	include "Kernel/ThreadTask.h"

#   include <vector>

namespace Menge 
{
    class ThreadWorkerInterface
        : public Factorable
    {
    public:
        virtual bool onWork() = 0;
        virtual void onDone() = 0;
    };

	class ThreadJob
		: public ThreadTask
	{
	public:
		ThreadJob();
		~ThreadJob();

    public:
        struct WorkerDesc
        {
            ThreadWorkerInterface * worker;
            bool stop;
        };
		
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep );

    public:
        void addWorker( ThreadWorkerInterface * _worker );
        void removeWorker( ThreadWorkerInterface * _worker );

    protected:
        bool _onMain() override;
        void _onUpdate() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
        size_t m_sleep;

        ThreadMutexInterface * m_mutex;

        typedef std::vector<WorkerDesc> TVectorWorkers; 
        TVectorWorkers m_workers;
        TVectorWorkers m_workersAdd;

        bool m_updating;
	};
}
