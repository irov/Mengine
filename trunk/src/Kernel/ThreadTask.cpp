#	include "ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::ThreadTask()
		: m_run(false)
		, m_finish(false)
		, m_complete(false)
		, m_successful(false)
		, m_cancel(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTask::~ThreadTask()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::main()
	{
		if( m_run == false ||
			m_finish == true ||
			m_complete == true )
		{
			return;
		}

		if( m_cancel == true )
		{
			m_successful = false;
			m_finish = true;

			return;
		}

		bool successful = this->_onMain();

		m_successful = successful;
		m_finish = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::run()
	{
		if( m_run == true ||
			m_cancel == true ||
			m_finish == true ||
			m_complete == true )
		{
			return false;
		}

		m_run = this->_onRun();

		return m_run;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isRun() const
	{
		return m_run;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isComplete() const
	{
		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isSuccessful() const
	{
		return m_successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::isCancel() const
	{
		return m_cancel;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onMain()
	{
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::_onRun()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::cancel()
	{
		if( m_cancel == true ||
			m_finish == true ||
			m_complete == true )
		{
			return false;
		}

		m_cancel = true;

		this->_onCancel();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onCancel()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTask::update()
	{
		if( m_run == false || m_complete == true )
		{
			return true;
		}

		this->_onUpdate();

		if( m_finish == true )
		{
			m_complete = true;

			this->_onComplete( m_successful );
		}

		return m_complete;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::_onUpdate()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////	
	void ThreadTask::_onComplete( bool _successful )
	{
		(void)_successful;
		//Empty
	}
}