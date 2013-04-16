#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge
{
    class LogServiceInterface;

    class ThreadTask;
    class ThreadTaskPacket;

    struct ThreadTaskHandle
    {
        ThreadTaskInterface * task;
        ThreadIdentity * identity;
    };

	class ThreadEngine
        : public ThreadServiceInterface
	{
	public:
		ThreadEngine();
		~ThreadEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( size_t _threadCount ) override;
        void finalize() override;

    public:
        size_t taskInProgress() const;
        bool isTaskOnProgress( ThreadTaskInterface * _task ) const;

    public:
        bool addTask( ThreadTaskInterface * _task ) override;
        bool joinTask( ThreadTaskInterface * _task ) override;
        bool cancelTask( ThreadTaskInterface * _task ) override;

    public:
        void addTaskPacket( ThreadTaskPacket * _taskPacket );

    public:
        void update() override;

    public:
        void sleep( unsigned int _ms ) override;

    protected:
        void testComplete_();
        bool joinTask_( ThreadTaskInterface * _task );

    protected:
        ThreadIdentity * getThreadIdentity_( ThreadTaskInterface * _task );

	protected:
        ServiceProviderInterface * m_serviceProvider;

        size_t m_threadCount;
        
        typedef std::vector<ThreadTaskHandle> TVectorThreadTaskHandle;
        TVectorThreadTaskHandle m_taskThread;
	};
}	// namespace Menge
