#include "PythonFileLogger.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PythonFileLogger::PythonFileLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PythonFileLogger::~PythonFileLogger()
    {
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
        size_t len = MENGINE_STRLEN( _msg );

        m_stream->write( _msg, len );
        m_stream->write( "\n", MENGINE_STATIC_STRING_LENGTH( "\n" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}