#include "CachalotLogger.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadWorkerHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VectorHelper.h"

#include "Config/StdString.h"

#include <clocale>
#include <iostream>

#include <io.h>
#include <fcntl.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::CachalotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    CachalotLogger::~CachalotLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool CachalotLogger::_initializeLogger()
    {
        ThreadMutexInterfacePtr mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        uint32_t CachalotPlugin_LoggerTime = CONFIG_VALUE( "CachalotPlugin", "Time", 2000 );

        if( Helper::createSimpleThreadWorker( STRINGIZE_STRING_LOCAL( "CachalotLogger" ), ETP_BELOW_NORMAL, CachalotPlugin_LoggerTime, nullptr, [this]()
        {
            this->process();
        }, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_finalizeLogger()
    {
        Helper::destroySimpleThreadWorker( STRINGIZE_STRING_LOCAL( "CachalotLogger" ) );

        m_mutex = nullptr;
    }    
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::setDSN( const String & _dsn )
    {
        m_dsn = _dsn;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & CachalotLogger::getDSN() const
    {
        return m_dsn;
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::_log( const LoggerMessage & _message )
    {
        m_mutex->lock();
        m_messages.emplace_back( _message );
        m_mutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::process()
    {
        uint32_t CachalotPlugin_BatchCount = CONFIG_VALUE( "CachalotPlugin", "BatchCount", 50 );

        m_mutex->lock();
        VectorMessages messages = Helper::exciseVector( m_messages, CachalotPlugin_BatchCount );
        m_mutex->unlock();

    }
    //////////////////////////////////////////////////////////////////////////
    void CachalotLogger::onHttpRequestComplete( const HttpResponseInterfacePtr & _response )
    {
        MENGINE_UNUSED( _response );

    }
    //////////////////////////////////////////////////////////////////////////
}
