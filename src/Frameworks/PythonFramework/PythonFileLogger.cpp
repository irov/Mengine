#include "PythonFileLogger.h"

#include "Interface/PlatformInterface.h"
#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/LoggerHelper.h"

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
    bool PythonFileLogger::initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath )
    {
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FACTORABLE );

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
            PlatformDateTime dateTime;
            DATETIME_SYSTEM()
                ->getLocalDateTime( &dateTime );

            Char timestamp[128] = {'\0'};
            size_t timestamp_len = Helper::makeLoggerTimestamp( dateTime, "[%02u:%02u:%02u:%04u] ", timestamp, 128 );

            m_stream->write( timestamp, timestamp_len );
        }

        size_t len = MENGINE_STRLEN( _msg );

        m_stream->write( _msg, len );
        m_stream->write( "\n", MENGINE_STATIC_STRING_LENGTH( "\n" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
