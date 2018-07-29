#include "ResourceOzzMesh.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/MemoryHelper.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/base/containers/vector_archive.h"
#include "ozz/base/io/archive_traits.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzMesh::ResourceOzzMesh()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzMesh::~ResourceOzzMesh()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzMesh::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceOzzMesh::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const Detail::Mesh & ResourceOzzMesh::getMesh() const
    {
        return m_mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzMesh::_compile()
    {
        const FileGroupInterfacePtr & category = this->getCategory();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( category, m_filePath, false );

        if( stream == nullptr )
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, "ResourceOzzMesh", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return false;
        }

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        ozz::io::MemoryStream ozz_stream;
        ozz_stream.Write( memory_buffer, memory_size );
        ozz_stream.Seek( 0, ozz::io::Stream::kSet );

        ozz::io::IArchive ozz_archive( &ozz_stream );

        ozz_archive >> m_mesh;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzMesh::_release()
    {
        //Empty
    }
}