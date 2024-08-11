#include "FileLogger.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/ContentHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileLogger::FileLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileLogger::~FileLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::setContent( const ContentInterfacePtr & _content )
    {
        m_content = _content;
    }
    //////////////////////////////////////////////////////////////////////////
    const ContentInterfacePtr & FileLogger::getContent() const
    {
        return m_content;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileLogger::_initializeLogger()
    {
        if( m_content == nullptr )
        {
            LOGGER_ERROR( "invalid initialize file logger invalid content" );

            return false;
        }

        OutputStreamInterfacePtr stream = m_content->openOutputStreamFile( false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "invalid open file logger '%s'"
                , Helper::getContentFullPath( m_content )
            );

            return false;
        }

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::_finalizeLogger()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();

            m_content->closeOutputStreamFile( m_stream );
            m_stream = nullptr;
        }

        m_content = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::_log( const LoggerRecordInterfacePtr & _record )
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

        if( message.flag & LFLAG_SYMBOLSTAMP )
        {
            ELoggerLevel level = message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            m_stream->write( &symbol, 1 );
            m_stream->write( " ", 1 );
        }

        if( message.flag & LFLAG_CATEGORYSTAMP )
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

        if( message.flag & LFLAG_FUNCTIONSTAMP )
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
    void FileLogger::_flush()
    {
        m_stream->flush();
    }
    //////////////////////////////////////////////////////////////////////////
    const OutputStreamInterfacePtr & FileLogger::getStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
}

