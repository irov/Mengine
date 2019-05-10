#include "ResourceOzzMesh.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"

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
    const Detail::Mesh & ResourceOzzMesh::getMesh() const
    {
        return m_mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzMesh::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

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