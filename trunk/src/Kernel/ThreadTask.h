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
		bool isComplete() const override;
		bool isCancel() const override;

	public:
		void main() override;

	public:
		bool run() override;
		bool cancel() override;
		bool update() override;
				
	protected:
		virtual bool _onMain();

	protected:
		virtual bool _onRun();
		virtual void _onCancel();
		virtual void _onUpdate();

		virtual void _onComplete( bool _successful );
		
	private:
		bool m_complete;
		volatile bool m_finish;
		volatile bool m_successful;
		volatile bool m_cancel;
	};

	typedef stdex::intrusive_ptr<ThreadTask> ThreadTaskPtr;
}
