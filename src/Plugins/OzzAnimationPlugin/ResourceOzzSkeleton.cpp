#include "ResourceOzzSkeleton.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ContentHelper.h"

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
        const ContentInterfacePtr & content = this->getContent();

        InputStreamInterfacePtr stream = content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open stream '%s'"
            , Helper::getContentFullPath( content ).c_str()
        );

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        content->closeInputStreamFile( stream );

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
    //////////////////////////////////////////////////////////////////////////
}