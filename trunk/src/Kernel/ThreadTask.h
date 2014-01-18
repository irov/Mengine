#	pragma once

#	include "Interface/ThreadSystemInterface.h"

namespace Menge 
{
	class ThreadTask
		: public ThreadTaskInterface
	{
	public:
		ThreadTask();
		~ThreadTask();
		
	public:	
		bool isComplete() const;
		bool isCancel() const;

	public:
		void main() override;
        void join() override;

	public:
		bool run() override;
		void cancel() override;
		bool update() override;
				
	protected:
		virtual bool _onMain();
		virtual void _onJoin();

	protected:
		virtual bool _onRun();
		virtual void _onCancel();
		virtual void _onUpdate();

		virtual void _onComplete( bool _successful );
		
	protected:
		volatile bool m_complete;
		volatile bool m_successful;
		volatile bool m_cancel;
	};

	typedef stdex::intrusive_ptr<ThreadTask> ThreadTaskPtr;
}
