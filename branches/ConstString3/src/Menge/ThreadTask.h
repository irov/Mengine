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
		: public ThreadListener
	{
	public:
		ThreadTask();
		virtual ~ThreadTask();
	
	public:
		virtual void destroy();
		
	public:	
		bool isComplete() const;
		bool isInterrupt() const;

	public:
		bool main() override;
		void cancel();
		void update();

	public:
		bool onMain();
	
	public:
		bool onRun();
		void onCancel();
		void onComplete();
		void onUpdate();
		void onInterrupt();

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
