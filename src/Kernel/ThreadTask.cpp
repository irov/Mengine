#include "ThreadTask.h"

#include "Kernel/ReferenceCounterGuardScope.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ThreadTask::ThreadTask()
        : m_run( false )
        , m_pending( true )
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
    void ThreadTask::process()
    {
        if( m_run == false ||
            m_finish == true ||
            m_pending == false )
        {
            return;
        }

        if( m_cancel == true )
        {
            m_successful = false;
            m_finish = true;

            return;
        }

        REFERENCECOUNTER_GUARD_SCOPE( this, "ThreadTask: %s"
            , Helper::getFactorableType( this ).c_str()
        );

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
            m_pending == false )
        {
            return false;
        }

        m_mutex = _mutex;

        m_run = true;

        if( this->_onThreadTaskRun() == false )
        {
            m_run = false;
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
        if( m_cancel.exchange( true ) == true )
        {
            return false;
        }

        if( m_pending == false )
        {
            return false;
        }

        this->_onThreadTaskCancel();

        if( m_run == false ||
            m_finish == true )
        {
            if( m_pending.exchange( false ) == false )
            {
                return false;
            }

            this->complete_( false );

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
        if( m_pending == false )
        {
            return m_complete;
        }

        if( m_finish == true )
        {
            return true;
        }

        if( m_run == false ||
            m_cancel == true )
        {
            return false;
        }

        this->_onThreadTaskUpdate();

        return m_finish;
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::finish()
    {
        if( m_pending.exchange( false ) == false )
        {
            return;
        }

        this->_onThreadTaskFinish();

        bool successful = m_successful;

        this->complete_( successful );
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::finally()
    {
        this->_onThreadTaskFinally();
    }
    //////////////////////////////////////////////////////////////////////////
    void ThreadTask::join()
    {
        if( m_pending.exchange( false ) == false )
        {
            return;
        }

        ThreadMutexInterfacePtr mutex = m_mutex;

        if( mutex != nullptr )
        {
            mutex->lock();
            mutex->unlock();
        }

        this->_onThreadTaskJoin();

        bool successful = m_successful;

        this->complete_( successful );
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
    void ThreadTask::complete_( bool _successful )
    {
        m_mutex = nullptr;
        m_successful = _successful;
        m_finish = true;
        m_complete = true;

        this->_onThreadTaskComplete( _successful );
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & ThreadTask::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
}
