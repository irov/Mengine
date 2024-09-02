#include "StreamLogger.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/LoggerHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::StreamLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::~StreamLogger()
    {
        MENGINE_ASSERTION_FATAL( m_stream == nullptr, "stream is not nullptr" );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::setStream( const OutputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;
    }
    //////////////////////////////////////////////////////////////////////////
    const OutputStreamInterfacePtr & StreamLogger::getStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StreamLogger::_initializeLogger()
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::_finalizeLogger()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        LoggerMessage message;
        _record->getMessage( &message );

        if( message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerShortDate( message.timestamp, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            m_stream->write( timestamp, timestampSize );
            m_stream->write( " ", 1 );
        }

        if(message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            m_stream->write( &symbol, 1 );
            m_stream->write( " ", 1 );
        }

        if(message.flag & LFLAG_CATEGORYSTAMP )
        {
            size_t category_size = StdString::strlen( message.category );

            m_stream->write( "[", 1 );
            m_stream->write( message.category, category_size );
            m_stream->write( "]", 1 );
            m_stream->write( " ", 1 );
        }

        if( message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( message.threadName, "|%s|", threadstamp, 0, 256 );
            m_stream->write( threadstamp, threadstampSize );
            m_stream->write( " ", 1 );
        }

        if( message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( message.function, message.line, "%s[%d]", functionstamp, 0, MENGINE_MAX_PATH );
            m_stream->write( functionstamp, functionstampSize );
            m_stream->write( " ", 1 );
        }

        m_stream->write( message.data, message.size );
        m_stream->write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::_flush()
    {
        m_stream->flush();
    }
    //////////////////////////////////////////////////////////////////////////
}