#	pragma once

#	include "Interface/ThreadSystemInterface.h"

#	include <vector>

namespace Menge 
{
	class Task;

	class TaskListener
	{
	public:
		virtual void onTaskRun( Task * _task ) = 0;
		virtual void onTaskComplete( Task * _task ) = 0;
	};

	class Task
		: public ThreadListener
	{
	public:
		Task();
		virtual ~Task();
	
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
		void addListener( TaskListener * _listener );

	protected:
		typedef std::vector<TaskListener *> TVectorTaskListener;
		TVectorTaskListener m_listeners;

		bool m_complete;
		bool m_interrupt;
	};
}
