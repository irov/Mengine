#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#   include <vector>

namespace Menge
{
	class ServiceProviderInterface;

	class ThreadListener
	{
	public:
		virtual bool main() = 0;
		//virtual void join() = 0;
	};

    class ThreadTaskInterface
        : public ThreadListener
    {
    public:	
        virtual bool isComplete() const = 0;
        virtual bool isInterrupt() const = 0;

    public:
        virtual bool onMain() = 0;

    public:
        virtual bool onRun() = 0;
        virtual void onCancel() = 0;
        virtual void onComplete() = 0;
        virtual void onUpdate() = 0;
        virtual void onInterrupt() = 0;

    public:
        virtual void cancel() = 0;
        virtual void update() = 0;
        virtual void destroy() = 0;
    };

    typedef std::vector<ThreadTaskInterface *> TVectorThreadTask;

	class ThreadIdentity
	{
	};

	class ThreadSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ThreadSystem")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ThreadIdentity * createThread( ThreadListener * _listener ) = 0;
		virtual void joinThread( ThreadIdentity * _thread ) = 0;
		virtual void sleep( unsigned int _ms ) = 0;
	};

#   define THREAD_SYSTEM( serviceProvider )\
    (Menge::getService<Menge::ThreadSystemInterface>(serviceProvider))

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
        virtual bool addTask( ThreadTaskInterface * _task ) = 0;
        virtual bool joinTask( ThreadTaskInterface * _task ) = 0;
        virtual bool cancelTask( ThreadTaskInterface * _task ) = 0;

    public:
        virtual void sleep( unsigned int _ms ) = 0;
    };

#   define THREAD_SERVICE( serviceProvider )\
    (Menge::getService<Menge::ThreadServiceInterface>(serviceProvider))
}