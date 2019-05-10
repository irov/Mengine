#include "ResourceOzzAnimation.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzAnimation::ResourceOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceOzzAnimation::~ResourceOzzAnimation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ozz::animation::Animation & ResourceOzzAnimation::getOzzAnimation() const
    {
        return m_animation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceOzzAnimation::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            return false;
        }

        MemoryInterfacePtr memory = Helper::createMemoryStream( stream, "ResourceOzzAnimation", __FILE__, __LINE__ );

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

        ozz_archive >> m_animation;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceOzzAnimation::_release()
    {
        //Empty
    }
}