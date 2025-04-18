#include "LoggerRecord.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoggerRecord::LoggerRecord()  
        : m_timestamp( 0 )
        , m_level( LM_SILENT )
        , m_flag( LFLAG_NONE )
        , m_filter( LFILTER_NONE )
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
        MENGINE_ASSERTION_MEMORY_PANIC( _message.category, "invalid initialize category" );
        MENGINE_ASSERTION_MEMORY_PANIC( _message.data, "invalid initialize data" );

        m_timestamp = _message.timestamp;
        StdString::strcpy_safe( m_category, _message.category, MENGINE_LOGGER_MAX_CATEGORY );
        m_thread = _message.thread;
        m_level = _message.level;
        m_flag = _message.flag;
        m_filter = _message.filter;
        m_color = _message.color;

        if( _message.file != nullptr )
        {
            StdString::strcpy_safe( m_file, _message.file, MENGINE_MAX_PATH );
        }
        else
        {
            m_file[0] = '\0';
        }

        if( _message.function != nullptr )
        {
            StdString::strcpy_safe( m_function, _message.function, MENGINE_CODE_MAX_FUNCTION_NAME );
        }
        else
        {
            m_function[0] = '\0';
        }

        m_line = _message.line;

        StdString::strzcpy_safe( m_data, _message.data, _message.size, MENGINE_LOGGER_MAX_MESSAGE );

        m_size = _message.size;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoggerRecord::getMessage( LoggerMessage * const _message ) const
    {
        _message->timestamp = m_timestamp;
        _message->category = m_category;
        _message->thread = m_thread;
        _message->level = m_level;
        _message->flag = m_flag;
        _message->filter = m_filter;
        _message->color = m_color;
        _message->file = m_file;
        _message->function = m_function;
        _message->line = m_line;
        _message->data = m_data;
        _message->size = m_size;
    }
    //////////////////////////////////////////////////////////////////////////
}