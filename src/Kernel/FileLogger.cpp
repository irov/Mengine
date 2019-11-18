#include "FileLogger.h"

#include "Kernel/Document.h"
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
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false );

        m_stream = stream;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::finalize()
    {
        m_fileGroup = nullptr;

        if( m_stream != nullptr )
        {
            m_stream->flush();
            m_stream = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FileLogger::log( ELoggerLevel _level, uint32_t _flag, const Char * _data, size_t _count )
    {
        MENGINE_UNUSED( _level );
        MENGINE_UNUSED( _flag );

        m_stream->write( _data, _count );
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
}

