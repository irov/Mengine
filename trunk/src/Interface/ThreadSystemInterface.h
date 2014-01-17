#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Utils/Factory/Factorable.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class ServiceProviderInterface;

    class ThreadTaskInterface
        : public Factorable
    {
    public:
		virtual void main() = 0;
		virtual void join() = 0;

	public:
		virtual bool run() = 0;
        virtual void cancel() = 0;
        virtual bool update() = 0;
    };

	class ThreadIdentity
	{
	};

    class ThreadMutexInterface
        : public Factorable
    {
    public:
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

	class ThreadSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ThreadSystem")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ThreadIdentity * createThread( ThreadTaskInterface * _listener, int _priority ) = 0;
		virtual bool joinThread( ThreadIdentity * _thread ) = 0;
		virtual void sleep( unsigned int _ms ) = 0;

        virtual ThreadMutexInterface * createMutex() = 0;
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
        virtual bool addTask( ThreadTaskInterface * _task, int _priority ) = 0;
        virtual void joinTask( ThreadTaskInterface * _task ) = 0;
        virtual void cancelTask( ThreadTaskInterface * _task ) = 0;

    public:
        virtual ThreadMutexInterface * createMutex() = 0;

    public:
        virtual void sleep( unsigned int _ms ) = 0;
    };

#   define THREAD_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::ThreadServiceInterface>(serviceProvider))
}