#   include "FileStore.h"

#   include "Metacode.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    void FileStore::loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_DataBlock::Meta_FileStore * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_FileStore *>(_meta);
        
        size_t count = metadata->get_Count();

        m_fileIDs.resize( count );

        const Metacode::Meta_DataBlock::Meta_FileStore::TVectorMeta_File & includes_file = metadata->get_IncludesFile();

        for( Metacode::Meta_DataBlock::Meta_FileStore::TVectorMeta_File::const_iterator 
            it = includes_file.begin(),
            it_end = includes_file.end();
        it != it_end;
        ++it )
        {
            const Metacode::Meta_DataBlock::Meta_FileStore::Meta_File & meta_file = *it;
            
            size_t index = meta_file.get_Index();

            FileID & fileId = m_fileIDs[index];

#   ifndef MENGE_MASTER_RELEASE
            //meta_file.get_Path( fileId );
#   else
            //meta_file.get_ID( fileId );
#   endif
        }
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