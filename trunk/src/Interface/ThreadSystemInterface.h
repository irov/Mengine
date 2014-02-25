#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Utils/Factory/Factorable.h"

#	include "Factory/FactorablePtr.h"

namespace Menge
{
    class ThreadTaskInterface
        : public FactorablePtr
    {
	public:
		virtual bool isComplete() const = 0;
		virtual bool isCancel() const = 0;

    public:
		virtual void main() = 0;

	public:
		virtual bool run() = 0;
        virtual bool cancel() = 0;
        virtual bool update() = 0;
    };

	typedef stdex::intrusive_ptr<ThreadTaskInterface> ThreadTaskInterfacePtr;

	class ThreadPoolInterface
		: public FactorablePtr
	{
	public:
		virtual void addTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual void cancel() = 0;
	};

	typedef stdex::intrusive_ptr<ThreadPoolInterface> ThreadPoolInterfacePtr;

	class ThreadIdentity
		: public FactorablePtr
	{
	};

	typedef stdex::intrusive_ptr<ThreadIdentity> ThreadIdentityPtr;

    class ThreadMutexInterface
        : public FactorablePtr
    {
    public:
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

	typedef stdex::intrusive_ptr<ThreadMutexInterface> ThreadMutexInterfacePtr;

	class ThreadSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ThreadSystem")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ThreadIdentityPtr createThread( const ThreadTaskInterfacePtr & _listener, int _priority ) = 0;
		virtual bool joinThread( const ThreadIdentityPtr & _thread ) = 0;
		virtual void sleep( unsigned int _ms ) = 0;

        virtual ThreadMutexInterfacePtr createMutex() = 0;
	};

#   define THREAD_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::ThreadSystemInterface>(serviceProvider))

    class ThreadServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ThreadService")

    public:
        virtual bool initialize( size_t _threadCount ) = 0;
        virtual void finalize() = 0;

    public:
        virtual void update() = 0;

    public:
        virtual bool addTask( const ThreadTaskInterfacePtr & _task, int _priority ) = 0;
        virtual void joinTask( const ThreadTaskInterfacePtr & _task ) = 0;
        virtual void cancelTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual ThreadPoolInterfacePtr runTaskPool() = 0;

    public:
        virtual ThreadMutexInterfacePtr createMutex() = 0;

    public:
        virtual void sleep( unsigned int _ms ) = 0;
    };

#   define THREAD_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::ThreadServiceInterface>(serviceProvider))
}