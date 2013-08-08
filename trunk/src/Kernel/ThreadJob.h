#	pragma once

#	include "Kernel/ThreadTask.h"

#   include <list>

namespace Menge 
{
    class ThreadWorkerInterface
        : public Factorable
    {
    public:
        virtual bool onWork( size_t _id ) = 0;
        virtual void onDone( size_t _id ) = 0;
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

        ThreadMutexInterface * m_mutexAdd;
        ThreadMutexInterface * m_mutexRemove;
        ThreadMutexInterface * m_mutexComplete;        

        size_t m_enumerator;

        typedef std::list<WorkerDesc> TWorkers; 
        TWorkers m_workers;
        TWorkers m_workersAdd;        
        TWorkers m_workersComplete;

        typedef std::list<size_t> TRemoveWorkers; 
        TRemoveWorkers m_workersRemove;
	};
}
