#include "ResourceOzzSkeleton.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzSkeleton::ResourceOzzSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzSkeleton::~ResourceOzzSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ozz::animation::Skeleton & ResourceOzzSkeleton::getOzzSkeleton() const
    {
        return m_skeleton;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzSkeleton::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, false );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false );

        const void * memory_buffer = memory->getBuffer();
        size_t memory_size = memory->getSize();

        ozz::io::MemoryStream ozz_stream;
        ozz_stream.Write( memory_buffer, memory_size );
        ozz_stream.Seek( 0, ozz::io::Stream::kSet );

        ozz::io::IArchive ozz_archive( &ozz_stream );

        ozz_archive >> m_skeleton;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzSkeleton::_release()
    {
        //Empty
    }
}