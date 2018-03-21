#pragma once

#include "Interface/ThreadInterface.h"

#include "Core/ServantBase.h"

namespace Mengine 
{
    //////////////////////////////////////////////////////////////////////////
	class ThreadTask
		: public ServantBase<ThreadTaskInterface>
	{
	public:
		ThreadTask();
		~ThreadTask();
		
	public:
		bool isRun() const override;
		bool isComplete() const override;
		bool isSuccessful() const override;
		bool isCancel() const override;

	public:
		void preparation() override;
		void main() override;

	public:
		bool run() override;
		bool cancel() override;
		bool update() override;
				
	protected:
		virtual void _onPreparation();
		virtual bool _onMain();

	protected:
		virtual bool _onRun();
		virtual void _onCancel();
		virtual void _onUpdate();

	protected:
		virtual void _onComplete( bool _successful );

	private:
		volatile bool m_run;
		volatile bool m_complete;
		volatile bool m_finish;
		volatile bool m_successful;
		volatile bool m_cancel;				
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTask> ThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}
