#include "FileLogger.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/LoggerHelper.h"

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
    void FileLogger::setFileGroup( const FileGroupInterfacePtr & _fileGroup )
    {
        m_fileGroup = _fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileLogger::getFileGroup() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & FileLogger::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileLogger::_initializeLogger()
    {
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePath, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::_finalizeLogger()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }

        m_fileGroup = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::log( const LoggerMessage & _message )
    {
        Char timestamp[256] = {'\0'};
        size_t timestampSize = Helper::makeLoggerTimestamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 256 );
        m_stream->write( timestamp, timestampSize );
        m_stream->write( " ", 1 );

        if( _message.category.empty() == false )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            m_stream->write( category_str, category_size );
            m_stream->write( " ", 1 );
        }

        const Char * data = _message.data;
        size_t size = _message.size;

        m_stream->write( data, size );
        m_stream->write( "\n", 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::flush()
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

