#	pragma once

#   include "Interface/ServantInterface.h"

#	include "Factory/Factorable.h"
#	include "Factory/FactorablePtr.h"

#	include "Core/ConstString.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskInterface
        : public ServantInterface
    {
	public:
		virtual bool isRun() const = 0;
		virtual bool isComplete() const = 0;
		virtual bool isSuccessful() const = 0;
		virtual bool isCancel() const = 0;

    public:
		virtual void preparation() = 0;
		virtual void main() = 0;

	public:
		virtual bool run() = 0;
        virtual bool cancel() = 0;
        virtual bool update() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskInterface> ThreadTaskInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class ThreadQueueInterface
		: public ServantInterface
	{
	public:
		virtual void addTask( const ThreadTaskInterfacePtr & _task ) = 0;

	public:
		virtual void cancel() = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadQueueInterface> ThreadQueueInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class ThreadIdentityInterface
		: public ServantInterface
	{
	public:
		virtual bool processTask( ThreadTaskInterface * _task ) = 0;
		virtual bool completeTask() = 0;

	public:
		virtual void join() = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadIdentityInterface> ThreadIdentityInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ThreadMutexInterface
        : public ServantInterface
    {
    public:
        virtual void lock() = 0;
        virtual void unlock() = 0;

	public:
		virtual bool try_lock() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadMutexInterface> ThreadMutexInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}