#include "ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTask::ThreadTask()
        : m_run( false )
        , m_finish( false )
        , m_complete( false )
        , m_successful( false )
        , m_cancel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ThreadTask::~ThreadTask()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::preparation()
    {
        this->_onPreparation();
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
    bool ThreadTask::run( const ThreadMutexInterfacePtr & _mutex )
    {
        if( m_run == true ||
            m_cancel == true ||
            m_finish == true ||
            m_complete == true )
        {
            return false;
        }

        m_mutex = _mutex;

        m_run = this->_onRun();

        if( m_run == false )
        {
            m_successful = false;
            m_finish = true;
        }

        return true;
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
    void ThreadTask::_onPreparation()
    {
        //Empty
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
	void ThreadTask::_onFinally()
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
            m_mutex = nullptr;

            m_complete = true;

            this->_onComplete( m_successful );
        }

        return m_complete;
    }
	//////////////////////////////////////////////////////////////////////////
	void ThreadTask::finally()
	{
        m_mutex = nullptr;

		this->_onFinally();
	}
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::join()
    {
        if( m_cancel == true ||
            m_finish == true ||
            m_complete == true )
        {
            return;
        }

        m_mutex->lock();
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onUpdate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////	
    void ThreadTask::_onComplete( bool _successful )
    {
        MENGINE_UNUSED( _successful );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ThreadTask::getMutex() const
    {
        return m_mutex;
    }
}