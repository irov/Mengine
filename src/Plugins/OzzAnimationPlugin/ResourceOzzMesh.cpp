#include "ResourceOzzMesh.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Document.h"
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
    const Detail::Mesh & ResourceOzzMesh::getMesh() const
    {
        return m_mesh;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzMesh::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, m_filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

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