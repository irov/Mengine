#   include "FileStore.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    void FileStore::loader( const Metabuf::Metadata * _parser )
    {
        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileStore::getFileID( size_t _index, FileID & _fileId ) const
    {
        if( m_fileIDs.size() <= _index )
        {
            return false;
        }

        _fileId = m_fileIDs[_index];

        return true;
    }
}