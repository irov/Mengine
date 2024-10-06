#include "PythonFileLogger.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/LoggerHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFileLogger::PythonFileLogger()
        : m_timestamp( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFileLogger::~PythonFileLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFileLogger::setTimestamp( bool _timestamp )
    {
        m_timestamp = _timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFileLogger::getTimestamp() const
    {
        return m_timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PythonFileLogger::initialize( const ContentInterfacePtr & _content )
    {
        OutputStreamInterfacePtr stream = _content->openOutputStreamFile( false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFileLogger::finalize()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void PythonFileLogger::write( const Char * _msg )
    {
        if( _msg == nullptr )
        {
            return;
        }

        if( m_timestamp == true )
        {
            Timestamp timestamp = Helper::getLocalTimestamp();

            Char shortDate[128 + 1] = {'\0'};
            size_t shortDateLen = Helper::makeLoggerShortDate( timestamp, "[%02u:%02u:%02u:%04u]", shortDate, 0, 128 );

            m_stream->write( shortDate, shortDateLen );
            m_stream->write( " ", 1 );
        }

        size_t len = StdString::strlen( _msg );

        m_stream->write( _msg, len );
        m_stream->write( "\n", MENGINE_STATIC_STRING_LENGTH( "\n" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
