#include "LoggerRecord.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerRecord::LoggerRecord()  
        : m_timestamp( 0 )
        , m_level( LM_SILENT )
        , m_flag( 0 )
        , m_filter( 0 )
        , m_color( 0 )
        , m_line( 0 )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoggerRecord::~LoggerRecord()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerRecord::initialize( const LoggerMessage & _message )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _message.category );
        MENGINE_ASSERTION_MEMORY_PANIC( _message.data );

        m_timestamp = _message.timestamp;
        MENGINE_STRNCPY( m_category, _message.category, MENGINE_LOGGER_MAX_CATEGORY );
        m_threadName = _message.threadName;
        m_level = _message.level;
        m_flag = _message.flag;
        m_filter = _message.filter;
        m_color = _message.color;

        if( _message.function != nullptr )
        {
            MENGINE_STRNCPY( m_function, _message.function, MENGINE_MAX_PATH );
        }
        else
        {
            m_function[0] = '\0';
        }

        m_line = _message.line;

        MENGINE_STRNCPY( m_data, _message.data, _message.size );

        m_size = _message.size;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerRecord::getMessage( LoggerMessage * const _message ) const
    {
        _message->timestamp = m_timestamp;
        _message->category = m_category;
        _message->threadName = m_threadName;
        _message->level = m_level;
        _message->flag = m_flag;
        _message->filter = m_filter;
        _message->color = m_color;
        _message->function = m_function;
        _message->line = m_line;
        _message->data = m_data;
        _message->size = m_size;
    }
    //////////////////////////////////////////////////////////////////////////
}