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
		bool isJoin() const;
		bool isCanceled() const;

	public:
		bool main() override;
		void join() override;
		void cancel();
		void update();

	public:
		bool onMain();
	
	public:
		bool onRun();
		void onCancel();
		void onComplete();
		void onJoin();
		void onUpdate();
		void onError();
	protected:
		virtual bool _onMain();

	protected:
		virtual bool _onRun();
		virtual void _onComplete();
		virtual void _onJoin();
		virtual void _onCancel();
		virtual void _onUpdate();
		virtual void _onError();
	public:
		void addListener( TaskListener * _listener );

	protected:
		bool m_join;
		bool m_complete;
		bool m_cancel;
		typedef std::vector<TaskListener *> TVectorTaskListener;
		TVectorTaskListener m_listeners;
	};
}
