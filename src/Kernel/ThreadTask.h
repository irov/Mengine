#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge 
{
	class ThreadTask;

	class ThreadTaskListener
	{
	public:
		virtual void onTaskRun( ThreadTask * _task ) = 0;
		virtual void onTaskComplete( ThreadTask * _task ) = 0;
	};

	class ThreadTask
		: public ThreadTaskInterface
	{
	public:
		ThreadTask();
		~ThreadTask();
		
	public:	
		bool isComplete() const override;
		bool isInterrupt() const override;

	public:
		bool main() override;
        void join() override;
		void cancel() override;
		void update() override;

	public:
		bool onMain() override;
	
	public:
		bool onRun() override;
		void onCancel() override;
		void onComplete() override;
		void onUpdate() override;
		void onInterrupt() override;

	protected:
		virtual bool _onMain();

	protected:
		virtual bool _onRun();
		virtual void _onComplete();
		virtual void _onCancel();
		virtual void _onUpdate();
		virtual void _onInterrupt();

	protected:
		bool m_complete;
		bool m_interrupt;
	};
}
