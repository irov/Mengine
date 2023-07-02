#include "ResourceOzzMesh.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

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
    ozz::span<const Detail::Mesh> ResourceOzzMesh::getMeshes() const
    {
        return ozz::make_span( m_meshes );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzMesh::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        ozz::io::MemoryStream ozz_stream;
        ozz_stream.Write( memory_buffer, memory_size );
        ozz_stream.Seek( 0, ozz::io::Stream::kSet );

        ozz::io::IArchive ozz_archive( &ozz_stream );

        while( ozz_archive.TestTag<Detail::Mesh>() == true )
        {
            m_meshes.resize( m_meshes.size() + 1 );
            ozz_archive >> m_meshes.back();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzMesh::_release()
    {
        m_meshes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}