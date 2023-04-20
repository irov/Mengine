#include "StreamLogger.h"

#include "Kernel/Assertion.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/LoggerHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::StreamLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StreamLogger::~StreamLogger()
    {
        MENGINE_ASSERTION_FATAL( m_stream == nullptr );
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
    void StreamLogger::log( const LoggerMessage & _message )
    {
        if( _message.flag & ELoggerFlag::LFLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%u]", functionstamp, 0, MENGINE_MAX_PATH );
            m_stream->write( functionstamp, functionstampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & LFLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            m_stream->write( timestamp, timestampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & LFLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            m_stream->write( threadstamp, threadstampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & LFLAG_SYMBOL )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            m_stream->write( &symbol, 1 );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & LFLAG_CATEGORY )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            m_stream->write( "[", 1 );
            m_stream->write( category_str, category_size );
            m_stream->write( "]", 1 );
            m_stream->write( " ", 1 );
        }

        m_stream->write( _message.data, _message.size );
        m_stream->write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void StreamLogger::flush()
    {
        m_stream->flush();
    }
    //////////////////////////////////////////////////////////////////////////
}

