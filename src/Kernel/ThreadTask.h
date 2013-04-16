#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include <vector>

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
		virtual ~ThreadTask();
	
	public:
		void destroy() override;
		
	public:	
		bool isComplete() const override;
		bool isInterrupt() const override;

	public:
		bool main() override;
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

	public:
		void addListener( ThreadTaskListener * _listener );

	protected:
		typedef std::vector<ThreadTaskListener *> TVectorTaskListener;
		TVectorTaskListener m_listeners;

		bool m_complete;
		bool m_interrupt;
	};
}
