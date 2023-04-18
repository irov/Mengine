#include "FileLogger.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"
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
        if( m_fileGroup == nullptr )
        {
            LOGGER_ERROR( "invalid initialize file logger invalid file group" );

            return false;
        }

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePath, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "invalid initialize file logger '%s'"
                , Helper::getFileGroupFullPath( m_fileGroup, m_filePath )
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

            Helper::closeOutputStreamFile( m_fileGroup, m_stream );
            m_stream = nullptr;
        }

        m_fileGroup = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::log( const LoggerMessage & _message )
    {
        if( _message.flag & ELF_FLAG_FUNCTIONSTAMP )
        {
            Char functionstamp[MENGINE_MAX_PATH] = {'\0'};
            size_t functionstampSize = Helper::makeLoggerFunctionStamp( _message.file, _message.line, "%s[%u]", functionstamp, 0, MENGINE_MAX_PATH );
            m_stream->write( functionstamp, functionstampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_TIMESTAMP )
        {
            Char timestamp[256] = {'\0'};
            size_t timestampSize = Helper::makeLoggerTimeStamp( _message.dateTime, "[%02u:%02u:%02u:%04u]", timestamp, 0, 256 );
            m_stream->write( timestamp, timestampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_THREADSTAMP )
        {
            Char threadstamp[256] = {'\0'};
            size_t threadstampSize = Helper::makeLoggerThreadStamp( "|%s|", threadstamp, 0, 256 );
            m_stream->write( threadstamp, threadstampSize );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_SYMBOL )
        {
            ELoggerLevel level = _message.level;

            Char symbol = Helper::getLoggerLevelSymbol( level );
            m_stream->write( &symbol, 1 );
            m_stream->write( " ", 1 );
        }

        if( _message.flag & ELF_FLAG_CATEGORY )
        {
            const Char * category_str = _message.category.c_str();
            size_t category_size = _message.category.size();

            m_stream->write( "[", 1 );
            m_stream->write( category_str, category_size );
            m_stream->write( "]", 1 );
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

