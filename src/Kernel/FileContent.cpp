#include "FileContent.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FileStreamHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FileContent::FileContent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileContent::~FileContent()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileContent::exist( bool _recursive ) const
    {
        bool result = m_fileGroup->existFile( m_filePath, _recursive );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr FileContent::createMemoryFile( bool _stream, bool _share, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr buffer = Helper::createMemoryFile( m_fileGroup, m_filePath, _stream, _share, _doc );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr FileContent::createMemoryFileString( bool _stream, bool _share, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr buffer = Helper::createMemoryFileString( m_fileGroup, m_filePath, _stream, _share, _doc );

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr FileContent::openInputStreamFile( bool _streaming, bool _share, const DocumentInterfacePtr & _doc )
    {
        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_fileGroup, m_filePath, _streaming, _share, _doc );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileContent::closeInputStreamFile( const InputStreamInterfacePtr & _stream )
    {
        bool successful = Helper::closeInputStreamFile( m_fileGroup, _stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileContent::openOutputStreamFile( bool _withTemp, const DocumentInterfacePtr & _doc )
    {
        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, m_filePath, _withTemp, _doc );
           
        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileContent::closeOutputStreamFile( const OutputStreamInterfacePtr & _stream )
    {
        bool successful = Helper::closeOutputStreamFile( m_fileGroup, _stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
