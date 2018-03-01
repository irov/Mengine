#	pragma once

#   include "Interface/ThreadInterface.h"

#   include "Interface/ServiceInterface.h"

#   include "Kernel/ThreadJob.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	const int32_t MENGINE_THREAD_PRIORITY_LOWEST = -2;
	const int32_t MENGINE_THREAD_PRIORITY_BELOW_NORMAL = -1;
	const int32_t MENGINE_THREAD_PRIORITY_NORMAL = 0;
	const int32_t MENGINE_THREAD_PRIORITY_ABOVE_NORMAL = 1;
	const int32_t MENGINE_THREAD_PRIORITY_HIGHEST = 2;
	const int32_t MENGINE_THREAD_PRIORITY_TIME_CRITICAL = 3;
    //////////////////////////////////////////////////////////////////////////
	class ThreadSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ThreadSystem")

	public:
		virtual bool avaliable() const = 0;

	public:
		virtual ThreadIdentityInterfacePtr createThread( int _priority, const char * _file, uint32_t _line ) = 0;

	public:
		virtual void sleep( uint32_t _ms ) = 0;

	public:
		virtual ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) = 0;

	public:
		virtual ptrdiff_t getCurrentThreadId() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
#   define THREAD_SYSTEM()\
    ((Menge::ThreadSystemInterface*)SERVICE_GET(Menge::ThreadSystemInterface))
    //////////////////////////////////////////////////////////////////////////
    class ThreadServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ThreadService")

	public:
		virtual bool avaliable() const = 0;

    public:
        virtual void update() = 0;

    public:
        virtual ThreadJobPtr createJob( uint32_t _sleep ) = 0;

	public:
		virtual bool createThread( const ConstString & _threadName, int _priority, const char * _file, uint32_t _line ) = 0;
		virtual bool destroyThread( const ConstString & _threadName ) = 0;

    public:
        virtual bool hasThread( const ConstString & _name ) const = 0;

    public:
        virtual bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) = 0;
        virtual bool joinTask( const ThreadTaskInterfacePtr & _task ) = 0;

    public:
        virtual void stopTasks() = 0;

	public:
		virtual ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize ) = 0;
		virtual void cancelTaskQueue( const ThreadQueueInterfacePtr & _queue ) = 0;

    public:
		virtual ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) = 0;

	public:
		virtual void sleep( uint32_t _ms ) = 0;

	public:
		virtual ptrdiff_t getCurrentThreadId() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define THREAD_SERVICE()\
    ((Menge::ThreadServiceInterface*)SERVICE_GET(Menge::ThreadServiceInterface))
    //////////////////////////////////////////////////////////////////////////
}