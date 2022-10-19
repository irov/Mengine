#include "ThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTask::ThreadTask()
        : m_run( false )
        , m_complete( false )
        , m_finish( false )
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
        this->_onThreadTaskPreparation();
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

        bool successful = this->_onThreadTaskProcess();

        if( m_cancel == true )
        {
            m_successful = false;
        }
        else
        {
            m_successful = successful;
        }

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

        m_run = this->_onThreadTaskRun();

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
    void ThreadTask::_onThreadTaskPreparation()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTask::_onThreadTaskProcess()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTask::_onThreadTaskRun()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTask::cancel()
    {
        if( m_cancel == true )
        {
            return false;
        }

        m_cancel = true;

        if( m_complete == true )
        {
            return false;
        }

        this->_onThreadTaskCancel();

        if( m_run == false ||
            m_finish == true )
        {
            m_successful = false;
            m_finish = true;

            if( m_complete == false )
            {
                m_complete = true;

                this->_onThreadTaskComplete( false );
            }

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskCancel()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskFinally()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool ThreadTask::update()
    {
        if( m_run == false )
        {
            return false;
        }

        if( m_complete == true || m_cancel == true )
        {
            return true;
        }

        this->_onThreadTaskUpdate();

        return m_finish;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::finish()
    {
        this->_onThreadTaskFinish();

        m_mutex = nullptr;

        if( m_complete == true )
        {
            return;
        }

        m_complete = true;

        this->_onThreadTaskComplete( m_successful );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::finally()
    {
        this->_onThreadTaskFinally();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::join()
    {
        if( m_run == false ||
            m_finish == true ||
            m_complete == true )
        {
            return;
        }

        bool completeProcess = false;

        m_mutex->lock();
        if( m_complete == false )
        {
            m_complete = true;
            completeProcess = true;
        }
        m_mutex->unlock();

        m_mutex = nullptr;

        if( completeProcess == true )
        {
            this->_onThreadTaskComplete( false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskUpdate()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskJoin()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskFinish()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::_onThreadTaskComplete( bool _successful )
    {
        MENGINE_UNUSED( _successful );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ThreadTask::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
}
