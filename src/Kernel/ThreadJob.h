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
            size_t id;
            bool stop;
            bool dead;
        };
		
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, size_t _sleep );

    public:
        size_t addWorker( ThreadWorkerInterface * _worker );
        void removeWorker( size_t _id );

    protected:
        bool _onMain() override;
        void _onUpdate() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
        size_t m_sleep;

        ThreadMutexInterface * m_mutex;
        ThreadMutexInterface * m_mutexAdd;

        size_t m_enumerator;

        typedef std::vector<WorkerDesc> TVectorWorkers; 
        TVectorWorkers m_workers;
        TVectorWorkers m_workersAdd;
	};
}
