#include "Win32HttpRequest.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequest::Win32HttpRequest()
        : m_hInternet( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32HttpRequest::~Win32HttpRequest()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequest::setHInternet( HINTERNET _hInternet )
    {
        m_hInternet = _hInternet;
    }
    //////////////////////////////////////////////////////////////////////////
    HINTERNET Win32HttpRequest::getHInternet() const
    {
        return m_hInternet;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequest::_onThreadTaskPreparation()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequest::_onThreadTaskRun()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32HttpRequest::_onThreadTaskProcess()
    {
        MENGINE_PROFILER_CATEGORY();

        MENGINE_ASSERTION_MEMORY_PANIC( m_response, "not setup 'response'" );
        MENGINE_ASSERTION_MEMORY_PANIC( m_receiver, "not setup 'receiver'" );

        bool successful = this->_onHttp();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32HttpRequest::_onThreadTaskComplete( bool _successful )
    {
        m_response->setSuccessful( _successful );

        m_mutex->lock();

        HttpResponseInterfacePtr response = std::move( m_response );
        HttpReceiverInterfacePtr receiver = std::move( m_receiver );
        receiver->onHttpRequestComplete( response );

        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
}
