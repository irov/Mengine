#	pragma once

#   include "Interface/ThreadInterface.h"

#   include "Interface/ServiceInterface.h"

#   include "Kernel/ThreadJob.h"

namespace Menge
{
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
#   define THREAD_SYSTEM( serviceProvider )\
    ((Menge::ThreadSystemInterface*)SERVICE_GET(serviceProvider, Menge::ThreadSystemInterface))
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
        virtual bool addTask( const ConstString & _threadName, const ThreadTaskInterfacePtr & _task ) = 0;
        virtual bool joinTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual ThreadQueueInterfacePtr runTaskQueue( const ConstString & _threadName, uint32_t _countThread, uint32_t _packetSize ) = 0;

    public:
		virtual ThreadMutexInterfacePtr createMutex( const char * _file, uint32_t _line ) = 0;

	public:
		virtual void sleep( uint32_t _ms ) = 0;

	public:
		virtual ptrdiff_t getCurrentThreadId() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define THREAD_SERVICE( serviceProvider )\
    ((Menge::ThreadServiceInterface*)SERVICE_GET(serviceProvider, Menge::ThreadServiceInterface))
    //////////////////////////////////////////////////////////////////////////
}