#include "FileLogger.h"

#include "Interface/LoggerServiceInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

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
    bool FileLogger::initialize()
    {
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePath, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        m_stream = stream;

        LOGGER_SERVICE()
            ->writeHistory( LoggerInterfacePtr::from( this ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::finalize()
    {
        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }

        m_fileGroup = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::log( ELoggerLevel _level, uint32_t _flag, uint32_t _color, const Char * _data, size_t _size )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );
        MENGINE_UNUSED( _color );

        m_stream->write( _data, _size );
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

